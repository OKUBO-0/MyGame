#include "Enemy.h"
using namespace KamataEngine;

Enemy::Enemy() {}
Enemy::~Enemy() {
    delete enemyModel_;
}

void Enemy::Initialize() {
    camera_.Initialize();
    enemyModel_ = Model::CreateFromOBJ("octopus");

    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };

    hp_ = 3;
    active_ = true;
}

void Enemy::Update() {
    if (!active_) return;

    // プレイヤーの方向を向く
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
        }

        worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);
    }

    // 前進（Z方向）
    worldTransform_.translation_.z -= speed_;
    if (worldTransform_.translation_.z < stopZ_) {
        worldTransform_.translation_.z = stopZ_;
    }

    worldTransform_.UpdateMatrix();
}

void Enemy::Draw() {
    if (!active_) return;
    enemyModel_->Draw(worldTransform_, camera_);
}

void Enemy::TakeDamage(int damage) {
    hp_ -= damage;
    if (hp_ <= 0) {
        Deactivate();
    }
}