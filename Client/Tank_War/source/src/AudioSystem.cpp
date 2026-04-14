#include "ECS/Systems/AudioSystem.h"
#include "ECS/Core/World.h"
#include "Manager/AudioDeviceManager.h"
#include "Manager/ResourceManager.h"

void AudioSystem::onAttach(World& world)
{
	m_world->Subscribe<AudioSystem, PickUpHealEvent>(
		[&](const PickUpHealEvent& event)->void
		{
			auto entity = event.picker;
			if (entity.hasComponent<TagTank>() && entity.hasComponent<TankProperty>())
			{
				spawnAudio(ResName::healAudio);
			}
		}
	);

	m_world->Subscribe<AudioSystem, DamageEvent>(
		[&](const DamageEvent& event)-> void
		{
			auto source = event.source;
			auto target = event.target;
			if (source.hasComponent<TagBullet>() && source.hasComponent<BulletCore>())
			{
				auto& bulletcore = source.getComponent<BulletCore>();
				if (bulletcore.type == WeaponType::Default)
					spawnAudio(ResName::defaultAttackedAudio);
			}
		}
	);

	m_world->Subscribe<AudioSystem, WeaponShootEvent>(
		[&](const WeaponShootEvent& event)->void
		{
			auto entity = event.source;
			if (entity.hasComponent<TagTank>() && entity.hasComponent<TankProperty>())
			{
				spawnAudio(ResName::defaultShootAudio);
			}
		}
	);
}

void AudioSystem::onDetach()
{
}

void AudioSystem::spawnAudio(const std::string& resname)
{
	if (auto audio = ResFactory->GetAudioRes(resname))
	{
		if (auto device = AudioDeviceManager::Instance()->GetDevice())
			device->PlayAudio(*audio, nullptr, (AudioChannelID)AudioChannelDef::SoundEffects_Channel);
	}
}
