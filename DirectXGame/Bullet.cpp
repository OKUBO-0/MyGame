#include "Bullet.h"
using namespace KamataEngine;

Bullet::Bullet() {}

Bullet::~Bullet() {
    delete model_;
}

void Bullet::Initialize(const Vector3& startPos, const Vector3& direction, float speed) {
    worldTransform_.Initialize();
    worldTransform_.translation_ = startPos;
    direction_ = direction;
    speed_ = speed;
    active_ = true;
    power_ = 1; // ★追加
    model_ = Model::CreateFromOBJ("bullet");
    camera_.Initialize();
}

void Bullet::Update() {
    if (!active_) return;

    // 弾の移動
    worldTransform_.translation_.x += direction_.x * speed_;
    worldTransform_.translation_.y += direction_.y * speed_;
    worldTransform_.translation_.z += direction_.z * speed_;

    // 画面外に出たら非アクティブ化
    if (worldTransform_.translation_.z > 50.0f || worldTransform_.translation_.z < -50.0f ||
        worldTransform_.translation_.x > 50.0f || worldTransform_.translation_.x < -50.0f) {
        active_ = false;
    }

    worldTransform_.UpdateMatrix();
}

void Bullet::Draw() {
    if (!active_) return;
    model_->Draw(worldTransform_, camera_);
}