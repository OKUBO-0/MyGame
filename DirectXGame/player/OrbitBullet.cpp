#include "OrbitBullet.h"
using namespace KamataEngine;

void OrbitBullet::Initialize(const Vector3& center, float radius, float angle, int32_t damage) {
    // Bullet の基本初期化（worldTransform_, power_, active_）
    Bullet::Initialize(center, damage);

    orbitRadius_ = radius;
    angle_ = angle;

    // モデル読み込み（以前の "Bullet" を使用）
    model_ = std::unique_ptr<Model>(Model::CreateFromOBJ("Bullet"));

    // 初期位置
    worldTransform_.translation_ = {
        center.x + std::cos(angle_) * orbitRadius_,
        center.y,
        center.z + std::sin(angle_) * orbitRadius_
    };

    worldTransform_.UpdateMatrix();
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

    // プレイヤー中心からの位置更新
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