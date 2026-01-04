#include "Bullet.h"
using namespace KamataEngine;

void Bullet::Initialize(const Vector3& startPos,
    const Vector3& direction,
    int32_t damage) {
    worldTransform_.Initialize();
    worldTransform_.translation_ = startPos;
    direction_ = direction;
    damage_ = damage;
    active_ = true;

    model_ = std::unique_ptr<Model>(Model::CreateFromOBJ("bullet"));
}

void Bullet::Update(const Vector3& playerPos) {
    if (!active_) return;

    // 移動
    worldTransform_.translation_.x += direction_.x * speed_;
    worldTransform_.translation_.y += direction_.y * speed_;
    worldTransform_.translation_.z += direction_.z * speed_;

    // 射程チェック
    float dx = worldTransform_.translation_.x - playerPos.x;
    float dy = worldTransform_.translation_.y - playerPos.y;
    float dz = worldTransform_.translation_.z - playerPos.z;
    float distSq = dx * dx + dy * dy + dz * dz;

    if (distSq > range_ * range_) {
        active_ = false;
    }

    worldTransform_.UpdateMatrix();
}

void Bullet::Draw(Camera* camera) {
    if (!active_ || !model_) return;
    model_->Draw(worldTransform_, *camera);
}