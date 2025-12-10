#include "HitParticle.h"
#include <cstdlib> // rand()
#include <cmath>   // cos, sin
using namespace KamataEngine;

void HitParticle::Initialize(const Vector3& pos) {
    // モデル生成（火花の見た目を表現する立方体モデルを読み込み）
    model_.reset(Model::CreateFromOBJ("cube"));

    // ワールド変換初期化（位置・回転・スケールの基準を設定）
    worldTransform_.Initialize();
    worldTransform_.translation_ = pos;                 // 火花の生成位置を設定
    worldTransform_.scale_ = { 0.2f, 0.2f, 0.2f };        // 初期サイズを小さめに設定

    // 初期状態の設定
    age_ = 0.0f;        // 経過時間リセット
    alpha_ = 1.0f;      // 完全不透明から開始
    active_ = true;     // 有効状態に設定

    // ランダム方向に速度を与える処理
    // 意図: 火花が自然に拡散するように、XZ平面方向にランダムな速度を付与し、
    //       少しだけY方向にも上昇成分を加える
    float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159f;
    float speed = 0.1f + static_cast<float>(rand()) / RAND_MAX * 0.2f;
    velocity_ = { cos(angle) * speed, 0.05f, sin(angle) * speed };

    // 行列更新（設定した位置・スケールを反映）
    worldTransform_.UpdateMatrix();
}

void HitParticle::Update() {
    constexpr float kDeltaTime = 0.016f; // 1フレーム時間（60FPS前提）
    age_ += kDeltaTime;

    // --- 寿命判定 ---
    // 意図: 一定時間経過後にパーティクルを非アクティブ化する
    if (age_ >= kLifetime) {
        active_ = false;
        return;
    }

    // --- 移動処理 ---
    // 意図: 初期速度に基づいて位置を更新し、火花が拡散する動きを表現する
    worldTransform_.translation_.x += velocity_.x;
    worldTransform_.translation_.y += velocity_.y;
    worldTransform_.translation_.z += velocity_.z;

    // --- 透明度減少処理 ---
    // 意図: 時間経過に応じて透明度を減少させ、火花が消えていく演出を行う
    alpha_ = 1.0f - (age_ / kLifetime);
    if (model_) {
        model_->SetAlpha(alpha_);
    }

    worldTransform_.UpdateMatrix();
}

void HitParticle::Draw(Camera* camera) {
    if (!active_ || !model_) return;
    model_->Draw(worldTransform_, *camera);
}