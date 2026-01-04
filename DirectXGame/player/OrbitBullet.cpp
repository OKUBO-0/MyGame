#include "OrbitBullet.h"
using namespace KamataEngine;

void OrbitBullet::Initialize(const Vector3& center, float radius, float angle, int32_t damage) {
    worldTransform_.Initialize();
    orbitRadius_ = radius;
    angle_ = angle;
    damage_ = damage;
    model_ = std::unique_ptr<Model>(Model::CreateFromOBJ("Bullet"));

    // 初期位置を設定
    worldTransform_.translation_ = {
        center.x + std::cos(angle_) * orbitRadius_,
        center.y,
        center.z + std::sin(angle_) * orbitRadius_
    };
}

bool OrbitBullet::CanHitEnemy(void* enemyPtr) {
    auto it = hitCooldowns_.find(enemyPtr);
    if (it == hitCooldowns_.end()) return true;
    return (it->second <= 0.0f);
}

void OrbitBullet::RegisterHit(void* enemyPtr) {
    hitCooldowns_[enemyPtr] = kHitInterval;
}

void OrbitBullet::Update(const Vector3& center) {
    if (!active_) return;

    // 角度を進める
    angle_ += angularSpeed_;

    // プレイヤー中心からの位置を更新
    worldTransform_.translation_ = {
        center.x + std::cos(angle_) * orbitRadius_,
        center.y,
        center.z + std::sin(angle_) * orbitRadius_
    };

    // クールタイム更新
    const float kDeltaTime = 0.016f;
    for (auto it = hitCooldowns_.begin(); it != hitCooldowns_.end();) {
        it->second -= kDeltaTime;
        if (it->second <= 0.0f) {
            it = hitCooldowns_.erase(it);
        }
        else {
            ++it;
        }
    }

    worldTransform_.UpdateMatrix();
}

void OrbitBullet::Draw(Camera* camera) {
    if (!active_ || !model_) return;
    model_->Draw(worldTransform_, *camera);
}