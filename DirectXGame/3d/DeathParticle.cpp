#include "DeathParticle.h"
using namespace KamataEngine;

void DeathParticle::Initialize(const Vector3& pos) {
    // 球体モデルで煙っぽく
    model_ = Model::CreateFromOBJ("bullet");
    worldTransform_.Initialize();
    worldTransform_.translation_ = pos;
    worldTransform_.scale_ = { 0.8f, 0.8f, 0.8f };

    // y+方向に上昇する速度を設定（横方向は弱め）
    velocity_ = {
        (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f,
        0.1f + static_cast<float>(rand()) / RAND_MAX * 0.1f,
        (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f
    };

    age_ = 0.0f;
    active_ = true;

    objectColor_ = new ObjectColor();
    objectColor_->Initialize();
    objectColor_->SetColor({ 0.5f, 0.5f, 0.5f, 1.0f }); // 灰色
}

void DeathParticle::Update() {
    const float dt = 0.016f;
    age_ += dt;
    if (age_ >= lifetime_) { active_ = false; return; }

    // 移動
    worldTransform_.translation_.x += velocity_.x;
    worldTransform_.translation_.y += velocity_.y;
    worldTransform_.translation_.z += velocity_.z;

    // 徐々に減速
    velocity_.x *= 0.95f;
    velocity_.y *= 0.98f;
    velocity_.z *= 0.95f;

    // スケールを拡大（煙が広がる）
    float scale = 0.8f + age_ * 0.3f;
    worldTransform_.scale_ = { scale, scale, scale };

    // アルファを減衰（煙が消える）
    float alpha = 1.0f - (age_ / lifetime_);
    objectColor_->SetColor({ 0.5f, 0.5f, 0.5f, alpha });

    worldTransform_.UpdateMatrix();
}

void DeathParticle::Draw(Camera* camera) {
    if (!active_) return;
    model_->Draw(worldTransform_, *camera, objectColor_);
}