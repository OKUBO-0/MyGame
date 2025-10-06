#include "Player.h"
using namespace KamataEngine;

Player::Player() {}

Player::~Player() {
    delete playerModel_;
    for (auto b : bullets_) {
        delete b;
    }
    bullets_.clear();
}

void Player::Initialize() {
    input_ = Input::GetInstance();
    camera_.Initialize();

    playerModel_ = Model::CreateFromOBJ("octopus");

    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
}

void Player::Update() {
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
    worldTransform_.rotation_.y = 0.0f;

    bulletTimer_ += 0.016f;

    Vector3 nearestDir = { 0.0f, 0.0f, 1.0f };
    float minDistSq = FLT_MAX;
    bool enemyInRange = false;

    if (enemyManager_) {
        for (auto enemy : enemyManager_->GetEnemies()) {
            if (!enemy->IsActive()) continue;

            Vector3 ePos = enemy->GetPosition();
            Vector3 pPos = worldTransform_.translation_;

            float dx = ePos.x - pPos.x;
            float dz = ePos.z - pPos.z;
            float distSq = dx * dx + dz * dz;

            if (distSq <= range_ * range_) {
                enemyInRange = true;

                if (distSq < minDistSq) {
                    minDistSq = distSq;
                    nearestDir = { dx, 0.0f, dz };
                }
            }
        }
    }

    if (bulletTimer_ >= bulletCooldown_ && enemyInRange) {
        float len = std::sqrt(nearestDir.x * nearestDir.x + nearestDir.z * nearestDir.z);
        if (len > 0.0f) {
            nearestDir.x /= len;
            nearestDir.z /= len;
        }

        Bullet* bullet = new Bullet();
        bullet->Initialize(worldTransform_.translation_, nearestDir, 0.5f);
        bullets_.push_back(bullet);

        bulletTimer_ = 0.0f;
    }

    for (auto bullet : bullets_) {
        bullet->Update();
    }

    worldTransform_.UpdateMatrix();
}

void Player::Draw() {
    playerModel_->Draw(worldTransform_, camera_);
    for (auto bullet : bullets_) {
        bullet->Draw();
    }
}