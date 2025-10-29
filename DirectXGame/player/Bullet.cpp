#include "Bullet.h"
using namespace KamataEngine;

Bullet::Bullet() {}

Bullet::~Bullet() {
    delete model_;
}

void Bullet::Initialize(const Vector3& startPos, const Vector3& direction, float speed) {
    // 初期化
    worldTransform_.Initialize();
    worldTransform_.translation_ = startPos;

    direction_ = direction;
    speed_ = speed;
    active_ = true;

    model_ = Model::CreateFromOBJ("bullet");
}

void Bullet::Update(const Vector3& playerPos) {
    if (!active_) return;

    // 弾の移動
    worldTransform_.translation_.x += direction_.x * speed_;
    worldTransform_.translation_.y += direction_.y * speed_;
    worldTransform_.translation_.z += direction_.z * speed_;

    // プレイヤー中心の範囲外判定
    const float limit = 50.0f;
    Vector3 pos = worldTransform_.translation_;
    float dx = pos.x - playerPos.x;
    float dy = pos.y - playerPos.y;
    float dz = pos.z - playerPos.z;
    float distSq = dx * dx + dy * dy + dz * dz;

    if (distSq > limit * limit) {
        active_ = false;
    }

    worldTransform_.UpdateMatrix();
}

void Bullet::Draw(Camera* camera) {
    if (!active_ || !model_) return;
    model_->Draw(worldTransform_, *camera);
}