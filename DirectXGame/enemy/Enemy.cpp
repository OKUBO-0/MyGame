#include "Enemy.h"
using namespace KamataEngine;

Enemy::Enemy() {}
Enemy::~Enemy() {
    delete enemyModel_;
}

void Enemy::Initialize() {
    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
    active_ = true;
}

void Enemy::SetModelByType(int type) {
    if (enemyModel_) {
        delete enemyModel_;
        enemyModel_ = nullptr;
    }

    switch (type) {
    case 0:
        enemyModel_ = Model::CreateFromOBJ("Enemy1");
        break;
    case 1:
        enemyModel_ = Model::CreateFromOBJ("Enemy2");
        break;
    case 2:
        enemyModel_ = Model::CreateFromOBJ("Enemy3");
        break;
    case 3:
        enemyModel_ = Model::CreateFromOBJ("Enemy4");
        break;
    default:
        enemyModel_ = Model::CreateFromOBJ("octopus");
        break;
    }
}

void Enemy::Update() {
    if (!active_) return;

    if (player_) {
        const Vector3& playerPos = player_->GetWorldPosition();
        Vector3 dir = {
            playerPos.x - worldTransform_.translation_.x,
            0.0f,
            playerPos.z - worldTransform_.translation_.z
        };

        float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
        if (len > 0.0f) {
            dir.x /= len;
            dir.z /= len;

            worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);
            worldTransform_.translation_.x += dir.x * speed_;
            worldTransform_.translation_.z += dir.z * speed_;
        }
    }

    worldTransform_.UpdateMatrix();
}

void Enemy::Draw(KamataEngine::Camera* camera) {
    if (!active_ || !enemyModel_) return;
    enemyModel_->Draw(worldTransform_, *camera);
}

void Enemy::TakeDamage(int damage) {
    hp_ -= damage;
    if (hp_ <= 0) {
        Deactivate();
    }
}