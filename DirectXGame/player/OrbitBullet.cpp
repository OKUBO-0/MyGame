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

    worldTransform_.UpdateMatrix();
}

void OrbitBullet::Draw(Camera* camera) {
    if (!active_ || !model_) return;
    model_->Draw(worldTransform_, *camera);
}