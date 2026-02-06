#include "ECS/Systems/PhysicsSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Components/AllComponent.h"
#include "tools.h"
#include "Helper/math2d.h"

// 获取碰撞点（世界坐标）
Pos2 getContactPoint(b2Contact* contact, const float PPM)
{
	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);

	// 通常使用第一个接触点
	b2Vec2 point = worldManifold.points[0];
	return Pos2(point.x * PPM, point.y * PPM);
}

// 获取碰撞法线
Vec2 getContactNormal(b2Contact* contact, const float PPM)
{
	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);

	b2Vec2 normal = worldManifold.normal;
	return Vec2(normal.x, normal.y);
}

PhysicsSystem::PhysicsSystem()
	: phyWorld(b2Vec2(0, 0))
{
}

void PhysicsSystem::fixedUpdate(float dt)
{
	checkNewEntity();
	collisionCheck(dt);
}

void PhysicsSystem::checkNewEntity()
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

void PhysicsSystem::collisionCheck(float dt)
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

	// 执行物理模拟
	phyWorld.Step(dt / 1000.f, 8, 10);

	processCollisions();

	// 同步位置回来
	for (auto entity : entities)
	{
		auto& physics = entity.getComponent<Physics>();
		if (physics.body)
		{
			if (auto* transform = entity.tryGetComponent<Transform>())
			{
				b2Vec2 pos = physics.body->GetPosition();
				transform->position.x = pos.x * PPM;
				transform->position.y = pos.y * PPM;
				transform->rotation = Tool::RadianToAngle(physics.body->GetAngle());

				// 子弹要反弹
				if (entity.hasComponent<TagBullet>())
				{
					b2Vec2 velocity = physics.body->GetLinearVelocity();
					velocity.Normalize();

					float radian = atan2f(velocity.y, velocity.x);
					float angle = Tool::RadianToAngle(radian);
					if (angle < 0)
						angle += 360.0f;
					transform->rotation = angle;
				}
			}
		}
	}
}

void PhysicsSystem::processCollisions()
{
	// 遍历所有接触点
	for (b2Contact* contact = phyWorld.GetContactList();
		contact != nullptr;
		contact = contact->GetNext())
	{
		// 检查是否正在接触
		if (contact->IsTouching())
		{
			b2Fixture* fixtureA = contact->GetFixtureA();
			b2Fixture* fixtureB = contact->GetFixtureB();

			b2Body* bodyA = fixtureA->GetBody();
			b2Body* bodyB = fixtureB->GetBody();

			// 从UserData获取Entity指针
			void* userDataA = reinterpret_cast<void*>(bodyA->GetUserData().pointer);
			void* userDataB = reinterpret_cast<void*>(bodyB->GetUserData().pointer);

			if (userDataA && userDataB)
			{
				Entity* entityA = static_cast<Entity*>(userDataA);
				Entity* entityB = static_cast<Entity*>(userDataB);

				PhysicsCollisionEvent collisionEvent{
					.entityA = *entityA,
					.entityB = *entityB,
					.point = getContactPoint(contact,PPM),
					.normal = getContactNormal(contact,PPM)
				};

				m_world->Emit<PhysicsCollisionEvent>(collisionEvent);
			}
		}
	}
}

// 创建物理体
void PhysicsSystem::createBody(Entity entity, Physics& physics, Transform& trans)
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

void PhysicsSystem::createBoundary(Entity entity, BoundaryPhysisc& boundaryphysics)
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
