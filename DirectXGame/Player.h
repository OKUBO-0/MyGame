#pragma once
#include <KamataEngine.h>
#include <vector>
#include "Bullet.h"
#include "EnemyManager.h"

class EnemyManager;
class Player {
public:
    Player();
    ~Player();

    void Initialize();
    void Update();
    void Draw();

    KamataEngine::Vector3 GetWorldPosition() const {
        return worldTransform_.translation_;
    }

    const std::vector<Bullet*>& GetBullets() const { return bullets_; }
    void SetEnemyManager(EnemyManager* manager) { enemyManager_ = manager; }

private:
    KamataEngine::Input* input_ = nullptr;
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Camera camera_;
    KamataEngine::Model* playerModel_ = nullptr;

    float speed_ = 0.3f;

    std::vector<Bullet*> bullets_;
    float bulletCooldown_ = 1.0f;
    float bulletTimer_ = 0.0f;

    EnemyManager* enemyManager_ = nullptr;

    float range_ = 30.0f; // 射程距離（任意の値）
};