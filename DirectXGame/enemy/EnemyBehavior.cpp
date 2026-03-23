#include "EnemyBehavior.h"
#include "Enemy.h"
#include "../player/Player.h"
#include <KamataEngine.h>
#include <cmath>

using namespace KamataEngine;

namespace {

class ChaseEnemyBehavior final : public IEnemyBehavior {
public:
    void Update(Enemy& enemy) override {
        Player* player = enemy.GetPlayer();
        if (!player) {
            return;
        }

        Vector3 position = enemy.GetPosition();
        Vector3 playerPos = player->GetWorldPosition();
        Vector3 dir = {
            playerPos.x - position.x,
            0.0f,
            playerPos.z - position.z
        };

        const float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
        if (len <= 0.0f) {
            return;
        }

        dir.x /= len;
        dir.z /= len;

        position.x += dir.x * enemy.GetSpeed();
        position.z += dir.z * enemy.GetSpeed();

        enemy.SetPosition(position);
        enemy.SetRotationY(std::atan2(dir.x, dir.z));
    }
};

class CircleApproachEnemyBehavior final : public IEnemyBehavior {
public:
    void Update(Enemy& enemy) override {
        Player* player = enemy.GetPlayer();
        if (!player) {
            return;
        }

        Vector3 position = enemy.GetPosition();
        Vector3 playerPos = player->GetWorldPosition();
        Vector3 toPlayer = {
            playerPos.x - position.x,
            0.0f,
            playerPos.z - position.z
        };

        float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z);
        if (dist > 0.001f) {
            toPlayer.x /= dist;
            toPlayer.z /= dist;
        }

        const Vector3 side = { -toPlayer.z, 0.0f, toPlayer.x };
        Vector3 finalDir = {
            toPlayer.x + side.x,
            0.0f,
            toPlayer.z + side.z
        };

        const float len = std::sqrt(finalDir.x * finalDir.x + finalDir.z * finalDir.z);
        if (len <= 0.001f) {
            return;
        }

        finalDir.x /= len;
        finalDir.z /= len;

        position.x += finalDir.x * enemy.GetSpeed();
        position.z += finalDir.z * enemy.GetSpeed();

        enemy.SetPosition(position);
        enemy.SetRotationY(std::atan2(finalDir.x, finalDir.z));
    }
};

} // namespace

std::unique_ptr<IEnemyBehavior> CreateEnemyBehaviorByType(int32_t type) {
    if (type == 2) {
        return std::make_unique<CircleApproachEnemyBehavior>();
    }

    return std::make_unique<ChaseEnemyBehavior>();
}
