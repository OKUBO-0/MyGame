#include "RippleEffect.h"
using namespace KamataEngine;

void RippleEffect::Initialize(const Vector3& pos) {
    // エフェクト用モデルを読み込み（effect.obj を利用）
    model_ = Model::CreateFromOBJ("ripples");

    // ワールドトランスフォームを初期化し、生成位置を設定
    worldTransform_.Initialize();
    worldTransform_.translation_ = pos;

    // 初期スケールを設定（小さな状態から始まる）
    worldTransform_.scale_ = { startScale_, startScale_, startScale_ };

    // パーティクルの寿命管理用変数を初期化
    age_ = 0.0f;     // 経過時間
    alpha_ = 1.0f;   // 完全不透明から開始
    active_ = true;  // 有効状態に設定
}

void RippleEffect::Update() {
    // 1フレームの経過時間（60FPS前提で固定値）
    const float dt = 0.016f;
    age_ += dt; // 経過時間を加算

    // 寿命を超えたら非アクティブ化して終了
    if (age_ >= lifetime_) {
        active_ = false;
        return;
    }

    // 寿命に対する進行度（0.0 → 1.0）
    float t = age_ / lifetime_;

    // スケールを徐々に拡大（開始サイズから終了サイズへ線形補間）
    float s = startScale_ + (endScale_ - startScale_) * t;
    worldTransform_.scale_ = { s, s, s };

    // 透明度を徐々に減少（不透明 → 完全透明）
    alpha_ = 1.0f - t;
    model_->SetAlpha(alpha_);

    // ワールド行列を更新（位置・スケール・回転を反映）
    worldTransform_.UpdateMatrix();
}

void RippleEffect::Draw(Camera* camera) {
    // 非アクティブ状態なら描画しない
    if (!active_) return;

    // 現在のワールド座標系とカメラを用いて描画
    model_->Draw(worldTransform_, *camera);
}