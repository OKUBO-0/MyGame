#include "Bullet.h"
using namespace KamataEngine;

Bullet::Bullet() {}
Bullet::~Bullet() {
    // 弾モデルを動的に生成しているため、破棄時に解放
    delete model_;
}

void Bullet::Initialize(const Vector3& startPos, const Vector3& direction, float speed) {
    // ワールドトランスフォームを初期化（位置・回転・スケールをリセット）
    worldTransform_.Initialize();

    // 弾の初期位置を設定
    worldTransform_.translation_ = startPos;

    // 弾の進行方向と速度を設定
    direction_ = direction;
    speed_ = speed;

    // 弾をアクティブ状態にする
    active_ = true;

    // 弾モデルを読み込み（bullet.obj を利用）
    model_ = Model::CreateFromOBJ("bullet");
}

void Bullet::Update(const Vector3& playerPos) {
    // 非アクティブ状態なら更新処理を行わない
    if (!active_) return;

    // 弾を進行方向へ移動
    worldTransform_.translation_.x += direction_.x * speed_;
    worldTransform_.translation_.y += direction_.y * speed_;
    worldTransform_.translation_.z += direction_.z * speed_;

    // プレイヤー位置からの距離を計算し、一定距離を超えたら弾を消す
    const float limit = 50.0f; // 弾の射程距離
    Vector3 pos = worldTransform_.translation_;
    float dx = pos.x - playerPos.x;
    float dy = pos.y - playerPos.y;
    float dz = pos.z - playerPos.z;
    float distSq = dx * dx + dy * dy + dz * dz;

    // 射程外に出たら非アクティブ化
    if (distSq > limit * limit) {
        active_ = false;
    }

    // ワールド行列を更新（位置・回転・スケールを反映）
    worldTransform_.UpdateMatrix();
}

void Bullet::Draw(Camera* camera) {
    // 非アクティブまたはモデル未設定なら描画しない
    if (!active_ || !model_) return;

    // 弾モデルを描画
    model_->Draw(worldTransform_, *camera);
}