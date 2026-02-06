#include "ECS/Systems/PredictionSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Components/AllComponent.h"
#include "Helper/Tools.h"
#include "Helper/math2d.h"
#include "Manager/UserInfoManager.h"

PredictionSystem::PredictionSystem()
	: phyWorld(b2Vec2(0, 0))
{
}

void PredictionSystem::fixedUpdate(float dt)
{
	checkNewEntity();
	collisionCheck(dt);
}

void PredictionSystem::checkNewEntity()
{
	{
		auto entities = m_world->getEntitiesWith<Physics, Transform>();
		for (auto entity : entities)
		{
			if (!entity.hasComponent<TagPhysiscCreate>())
			{
				auto& physics = entity.getComponent<Physics>();
				auto& trans = entity.getComponent<Transform>();
				createBody(entity, physics, trans);
				entity.addComponent<TagPhysiscCreate>();
			}
		}
	}

	{

		auto entities = m_world->getEntitiesWith<BoundaryPhysisc>();
		for (auto entity : entities)
		{
			if (!entity.hasComponent<TagPhysiscCreate>())
			{
				auto& boundary = entity.getComponent<BoundaryPhysisc>();
				createBoundary(entity, boundary);
				entity.addComponent<TagPhysiscCreate>();
			}
		}
	}
}

void PredictionSystem::collisionCheck(float dt)
{
	auto entities = m_world->getEntitiesWith<Physics, Transform>();

	for (auto entity : entities)
	{
		auto& physics = entity.getComponent<Physics>();
		if (physics.body)
		{
			auto& trans = entity.getComponent<Transform>();

			// 同步位置
			b2Vec2 physicsPos(
				trans.position.x / PPM,
				trans.position.y / PPM
			);
			physics.body->SetTransform(physicsPos, Tool::AngleToRadian(trans.rotation));

			if (auto* tankproperty = entity.tryGetComponent<TankProperty>())
			{
				if (tankproperty->playerId == UserInfoManager::Instance()->usertoken())
				{
					if (auto* move = entity.tryGetComponent<Movement>())
					{
						Vec2 direction = { cos(Tool::AngleToRadian(trans.rotation)),sin(Tool::AngleToRadian(trans.rotation)) };

						b2Vec2 velocity(
							direction.x * move->currentMoveSpeed / PPM,
							direction.y * move->currentMoveSpeed / PPM
						);
						physics.body->SetLinearVelocity(velocity);
						physics.body->SetAngularVelocity(Tool::AngleToRadian(move->currentRotationSpeed));
					}
				}
			}
		}
	}

	// 执行物理模拟
	phyWorld.Step(dt / 1000.f, 8, 10);

	// 同步位置回来
	for (auto entity : entities)
	{
		if (auto* tankproperty = entity.tryGetComponent<TankProperty>())
		{
			if (tankproperty->playerId == UserInfoManager::Instance()->usertoken())
			{
				if (auto* transform = entity.tryGetComponent<Transform>())
				{
					auto& physics = entity.getComponent<Physics>();
					if (physics.body)
					{
						b2Vec2 pos = physics.body->GetPosition();
						transform->position.x = pos.x * PPM;
						transform->position.y = pos.y * PPM;
						transform->rotation = Tool::RadianToAngle(physics.body->GetAngle());
					}
				}
			}
		}
	}
}

// 创建物理体
void PredictionSystem::createBody(Entity entity, Physics& physics, Transform& trans)
{
	physics.world = &phyWorld;

	// 获取位置
	Pos2 position = trans.position;

	// 创建刚体定义
	b2BodyDef bodyDef;

	if (physics.bodyType == Physics::BodyType::Dynamic)
		bodyDef.type = b2_dynamicBody;
	else if (physics.bodyType == Physics::BodyType::Static)
		bodyDef.type = b2_staticBody;
	else
		bodyDef.type = b2_kinematicBody;

	if (bodyDef.type == b2_dynamicBody)
	{
		auto* tankproperty = entity.tryGetComponent<TankProperty>();
		if (!tankproperty || tankproperty->playerId != UserInfoManager::Instance()->usertoken())
		{
			bodyDef.type = b2_staticBody;
		}
	}

	bodyDef.position.Set(position.x / PPM, position.y / PPM);
	bodyDef.angle = Tool::AngleToRadian(trans.rotation);
	bodyDef.fixedRotation = physics.fixedRotation;
	bodyDef.bullet = physics.isBullet;

	bodyDef.allowSleep = false;  //始终活跃
	bodyDef.awake = true;        //保持唤醒状态

	// 创建刚体
	physics.body = phyWorld.CreateBody(&bodyDef);
	b2Vec2 physicsPos(
		trans.position.x / PPM,
		trans.position.y / PPM
	);
	physics.body->GetUserData().pointer = reinterpret_cast<uintptr_t>(new Entity(entity));

	// 创建碰撞形状
	b2FixtureDef fixtureDef;
	if (physics.shape == Physics::Shape::Round) {
		// 圆形
		b2CircleShape* circle = new b2CircleShape();
		circle->m_radius = physics.size.x / 2.0f / PPM;
		fixtureDef.shape = circle;
	}
	else {
		// 矩形
		b2PolygonShape* box = new b2PolygonShape();
		box->SetAsBox(
			physics.size.x / 2.0f / PPM,
			physics.size.y / 2.0f / PPM
		);
		fixtureDef.shape = box;
	}

	fixtureDef.density = physics.density;        // 密度
	fixtureDef.friction = physics.friction;      // 摩擦
	fixtureDef.restitution = physics.restitution;// 弹性
	fixtureDef.isSensor = physics.isSensor;      // 传感器模式

	fixtureDef.filter.groupIndex = physics.groupIndex;
	fixtureDef.filter.categoryBits = 0x0001;     // 碰撞类别
	fixtureDef.filter.maskBits = 0xFFFF;         // 碰撞掩码

	// 应用 Fixture
	physics.body->CreateFixture(&fixtureDef);
}

void PredictionSystem::createBoundary(Entity entity, BoundaryPhysisc& boundaryphysics)
{
	// 创建一个静态刚体作为所有边界的容器
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(0, 0);

	boundaryphysics.boundaryBody = phyWorld.CreateBody(&bodyDef);

	// 1. 上边缘
	b2EdgeShape topEdge;
	topEdge.SetTwoSided(
		b2Vec2(boundaryphysics.x1 / PPM, boundaryphysics.y1 / PPM),   // 左端点
		b2Vec2(boundaryphysics.x2 / PPM, boundaryphysics.y1 / PPM)    // 右端点
	);

	// 2. 下边缘
	b2EdgeShape bottomEdge;
	bottomEdge.SetTwoSided(
		b2Vec2(boundaryphysics.x1 / PPM, boundaryphysics.y2 / PPM),  // 左端点
		b2Vec2(boundaryphysics.x2 / PPM, boundaryphysics.y2 / PPM)    // 右端点
	);

	// 3. 左边缘
	b2EdgeShape leftEdge;
	leftEdge.SetTwoSided(
		b2Vec2(boundaryphysics.x1 / PPM, boundaryphysics.y1 / PPM),   // 上端点
		b2Vec2(boundaryphysics.x1 / PPM, boundaryphysics.y2 / PPM)   // 下端点
	);

	// 4. 右边缘
	b2EdgeShape rightEdge;
	rightEdge.SetTwoSided(
		b2Vec2(boundaryphysics.x2 / PPM, boundaryphysics.y1 / PPM),    // 上端点
		b2Vec2(boundaryphysics.x2 / PPM, boundaryphysics.y2 / PPM)    // 下端点
	);

	// 创建所有 Fixture
	b2FixtureDef fixtureDef;
	fixtureDef.density = 0.0f;
	fixtureDef.friction = 0.8f;
	fixtureDef.restitution = 0.1f;

	// 添加四个边缘
	fixtureDef.shape = &topEdge;
	boundaryphysics.boundaryBody->CreateFixture(&fixtureDef);

	fixtureDef.shape = &bottomEdge;
	boundaryphysics.boundaryBody->CreateFixture(&fixtureDef);

	fixtureDef.shape = &leftEdge;
	boundaryphysics.boundaryBody->CreateFixture(&fixtureDef);

	fixtureDef.shape = &rightEdge;
	boundaryphysics.boundaryBody->CreateFixture(&fixtureDef);

	boundaryphysics.boundaryBody->GetUserData().pointer = reinterpret_cast<uintptr_t>(new Entity(entity));

}
