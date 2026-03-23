#pragma once

#include <memory>

class Enemy;

class IEnemyBehavior {
public:
    virtual ~IEnemyBehavior() = default;
    virtual void Update(Enemy& enemy) = 0;
};

std::unique_ptr<IEnemyBehavior> CreateEnemyBehaviorByType(int32_t type);
