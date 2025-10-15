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
    power_ = 1;
    active_ = true;

    model_ = Model::CreateFromOBJ("bullet");
    camera_.Initialize();
}

void Bullet::Update() {
    if (!active_) return;

    // 弾の移動
    worldTransform_.translation_.x += direction_.x * speed_;
    worldTransform_.translation_.y += direction_.y * speed_;
    worldTransform_.translation_.z += direction_.z * speed_;

    // 範囲外で非アクティブ化
    const float limit = 50.0f;
    const Vector3& pos = worldTransform_.translation_;
    if (pos.z > limit || pos.z < -limit || pos.x > limit || pos.x < -limit) {
        active_ = false;
    }

    worldTransform_.UpdateMatrix();
}

void Bullet::Draw() {
    if (!active_) return;
    model_->Draw(worldTransform_, camera_);
}