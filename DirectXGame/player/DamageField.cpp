#include "DamageField.h"
using namespace KamataEngine;

void DamageField::Initialize(float radius, int32_t damage) {
    worldTransform_.Initialize();
    radius_ = radius;
    damage_ = damage;
    model_ = std::unique_ptr<Model>(Model::CreateFromOBJ("damageField"));

    worldTransform_.scale_ = { radius_, 0.1f, radius_ }; // 円形フィールドを表現
}

void DamageField::Update(const Vector3& center) {
    if (!active_) return;

    // プレイヤー中心に追従
    worldTransform_.translation_ = center;
    worldTransform_.UpdateMatrix();
}

void DamageField::Draw(Camera* camera) {
    if (!active_ || !model_) return;
    model_->Draw(worldTransform_, *camera);
}