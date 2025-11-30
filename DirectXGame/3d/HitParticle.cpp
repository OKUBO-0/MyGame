// HitParticle.cpp
#include "HitParticle.h"
#include <cstdlib> // rand()
using namespace KamataEngine;

void HitParticle::Initialize(const Vector3& pos) {
    model_ = Model::CreateFromOBJ("cube"); // 火花用モデル

    worldTransform_.Initialize();
    worldTransform_.translation_ = pos;
    worldTransform_.scale_ = { 0.2f, 0.2f, 0.2f };

    age_ = 0.0f;
    alpha_ = 1.0f;
    active_ = true;

    // ランダム方向に速度を与える（XZ平面＋少しY方向）
    float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159f;
    float speed = 0.1f + static_cast<float>(rand()) / RAND_MAX * 0.2f;
    velocity_ = { cos(angle) * speed, 0.05f, sin(angle) * speed };

    // 初期位置を反映
    worldTransform_.UpdateMatrix();
}

void HitParticle::Update() {
    const float dt = 0.016f;
    age_ += dt;

    if (age_ >= lifetime_) {
        active_ = false;
        return;
    }

    // 移動
    worldTransform_.translation_.x += velocity_.x;
    worldTransform_.translation_.y += velocity_.y;
    worldTransform_.translation_.z += velocity_.z;

    // 徐々に透明化
    alpha_ = 1.0f - (age_ / lifetime_);
    model_->SetAlpha(alpha_);

    worldTransform_.UpdateMatrix();
}

void HitParticle::Draw(Camera* camera) {
    if (!active_) return;
    model_->Draw(worldTransform_, *camera);
}