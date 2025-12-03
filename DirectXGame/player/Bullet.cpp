#include "Bullet.h"
using namespace KamataEngine;

Bullet::Bullet() {}

Bullet::~Bullet() {
    /// <summary>
    /// 弾モデルを動的に生成しているため、破棄時に解放
    /// </summary>
    delete model_;
}

void Bullet::Initialize(const Vector3& startPos, const Vector3& direction, float speed) {
    /// <summary>
    /// ワールドトランスフォームを初期化（位置・回転・スケールをリセット）
    /// </summary>
    worldTransform_.Initialize();

    /// <summary>
    /// 弾の初期位置を設定
    /// </summary>
    worldTransform_.translation_ = startPos;

    /// <summary>
    /// 弾の進行方向と速度を設定
    /// </summary>
    direction_ = direction;
    speed_ = speed;

    /// <summary>
    /// 弾をアクティブ状態にする
    /// </summary>
    active_ = true;

    /// <summary>
    /// 弾モデルを読み込み（bullet.obj を利用）
    /// </summary>
    model_ = Model::CreateFromOBJ("bullet");
}

void Bullet::Update(const Vector3& playerPos) {
    /// <summary>
    /// 非アクティブ状態なら更新処理を行わない
    /// </summary>
    if (!active_) { return; }

    /// <summary>
    /// 弾を進行方向へ移動
    /// </summary>
    worldTransform_.translation_.x += direction_.x * speed_;
    worldTransform_.translation_.y += direction_.y * speed_;
    worldTransform_.translation_.z += direction_.z * speed_;

    /// <summary>
    /// プレイヤー位置からの距離を計算し、一定距離を超えたら弾を消す
    /// </summary>
    const float kLimitDistance = 50.0f; ///< 弾の射程距離
    Vector3 pos = worldTransform_.translation_;
    float dx = pos.x - playerPos.x;
    float dy = pos.y - playerPos.y;
    float dz = pos.z - playerPos.z;
    float distSq = dx * dx + dy * dy + dz * dz;

    /// <summary>
    /// 射程外に出たら非アクティブ化
    /// </summary>
    if (distSq > kLimitDistance * kLimitDistance) {
        active_ = false;
    }

    /// <summary>
    /// ワールド行列を更新（位置・回転・スケールを反映）
    /// </summary>
    worldTransform_.UpdateMatrix();
}

void Bullet::Draw(Camera* camera) {
    /// <summary>
    /// 非アクティブまたはモデル未設定なら描画しない
    /// </summary>
    if (!active_ || !model_) { return; }

    /// <summary>
    /// 弾モデルを描画
    /// </summary>
    model_->Draw(worldTransform_, *camera);
}