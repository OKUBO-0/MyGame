#include "Drone.h"
#include "../ModelCache.h"
using namespace KamataEngine;

void Drone::Initialize(const Vector3& offset) {
    offset_ = offset;
    worldTransform_.Initialize();
    worldTransform_.scale_ = { 0.5f, 0.5f, 0.5f };
    model_ = ModelCache::Get("octopus");
}

void Drone::Update(const Vector3& playerPos,
    const std::vector<std::unique_ptr<Enemy>>& enemies,
    float& fireTimer, float fireInterval)
{
    const float dt = 0.016f;

    // 浮遊アニメーション用の時間
    static float time = 0.0f;
    time += dt * 60.0f; // 60fps 基準で増加

    // 上下浮遊（サイン波）
    float floatY = std::sinf(time * 0.05f) * 0.5f;
    // 振幅0.5、速度0.05 → 自然なふわふわ

    // プレイヤーの移動には追従するが、回転には追従しない
    worldTransform_.translation_ = {
        playerPos.x + offset_.x,
        playerPos.y + offset_.y + floatY, // ← 浮遊を加算
        playerPos.z + offset_.z
    };

    // --- 敵探索 ---
    fireTimer += dt;

    Enemy* target = nullptr;
    float minDistSq = range_ * range_;

    for (auto& e : enemies) {
        if (!e->IsActive()) continue;

        Vector3 ePos = e->GetPosition();
        float dx = ePos.x - worldTransform_.translation_.x;
        float dz = ePos.z - worldTransform_.translation_.z;
        float distSq = dx * dx + dz * dz;

        if (distSq < minDistSq) {
            minDistSq = distSq;
            target = e.get();
        }
    }

    // --- 敵がいる場合は向きを変えて発射 ---
    if (target && fireTimer >= fireInterval) {

        Vector3 dir = {
            target->GetPosition().x - worldTransform_.translation_.x,
            0.0f,
            target->GetPosition().z - worldTransform_.translation_.z
        };

        float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
        if (len > 0.0f) {
            dir.x /= len;
            dir.z /= len;
        }

        // 敵の方向へ向く
        float angleToTarget = std::atan2(dir.x, dir.z);
        worldTransform_.rotation_.y = angleToTarget;

        // 弾発射
        auto b = std::make_unique<NormalBullet>();
        b->InitializeForward(worldTransform_.translation_, dir);
        bullets_.push_back(std::move(b));

        fireTimer = 0.0f;
    }

    // --- 弾更新 ---
    for (auto& b : bullets_) {
        b->Update(worldTransform_.translation_);
    }

    bullets_.erase(
        std::remove_if(bullets_.begin(), bullets_.end(),
            [](auto& b) { return !b->IsActive(); }),
        bullets_.end()
    );

    worldTransform_.UpdateMatrix();
}

void Drone::Draw(Camera* camera) {
    if (model_) {
        model_->Draw(worldTransform_, *camera);
    }
    for (auto& b : bullets_) {
        b->Draw(camera);
    }
}
