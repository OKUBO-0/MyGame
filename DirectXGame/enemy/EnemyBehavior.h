#pragma once

#include <memory>

namespace DirectXGame {

class Enemy;

class IEnemyBehavior {
public:
    virtual ~IEnemyBehavior() = default;
    virtual void Update(Enemy& enemy) = 0;
};

std::unique_ptr<IEnemyBehavior> CreateEnemyBehaviorByType(int32_t type);

} // namespace DirectXGame
