#pragma once

#include "ECS/Components/PlayerInput.h"
#include "ECS/Core/System.h"
#include "GameInstance.h"

class CheckGameOverSystem : public System
{
public:
    CheckGameOverSystem();
    virtual ~CheckGameOverSystem();
    virtual void onAttach(World& world);
    virtual void preUpdate(float deltaTime);

    void handleProcessKillEvent(const TankDestroyedEvent &event);

    void SetInstance(std::shared_ptr<GameInstance> instance);
    void SetGameTimeLimit(uint64_t gameTimeLimitSecond);

private:
    std::shared_ptr<GameInstance> m_instance;
    float _gameTimeLimit = 300;     //限制游戏时长
};