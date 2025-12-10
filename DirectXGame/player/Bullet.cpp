#include "Bullet.h"
using namespace KamataEngine;

void Bullet::Initialize(const Vector3& startPos, const Vector3& direction, float speed) {
    // --- ワールドトランスフォーム初期化 ---
    worldTransform_.Initialize();

    // --- 弾の初期位置設定 ---
    worldTransform_.translation_ = startPos;

    // --- 弾の進行方向と速度設定 ---
    direction_ = direction;
    speed_ = speed;

    // --- アクティブ化 ---
    active_ = true;

    // --- 弾モデル生成（bullet.obj を利用） ---
    model_ = std::unique_ptr<Model>(Model::CreateFromOBJ("bullet"));
}

void Bullet::Update(const Vector3& playerPos) {
    // --- 非アクティブなら更新処理を行わない ---
    if (!active_) { return; }

    // --- 弾を進行方向へ移動 ---
    worldTransform_.translation_.x += direction_.x * speed_;
    worldTransform_.translation_.y += direction_.y * speed_;
    worldTransform_.translation_.z += direction_.z * speed_;

    // --- プレイヤー位置からの距離を計算 ---
    const float kLimitDistance = 50.0f; // 弾の射程距離
    Vector3 pos = worldTransform_.translation_;
    float dx = pos.x - playerPos.x;
    float dy = pos.y - playerPos.y;
    float dz = pos.z - playerPos.z;
    float distSq = dx * dx + dy * dy + dz * dz;

    // --- 射程外なら非アクティブ化 ---
    if (distSq > kLimitDistance * kLimitDistance) {
        active_ = false;
    }

    // --- ワールド行列更新 ---
    worldTransform_.UpdateMatrix();
}

void Bullet::Draw(Camera* camera) {
    // --- 非アクティブまたはモデル未設定なら描画しない ---
    if (!active_ || !model_) { return; }

    // --- 弾モデル描画 ---
    model_->Draw(worldTransform_, *camera);
}