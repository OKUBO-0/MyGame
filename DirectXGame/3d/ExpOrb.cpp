#include "ExpOrb.h"
#include <random>
#include <cmath>
using namespace KamataEngine;

void ExpOrb::Initialize(const Vector3& pos, int32_t expValue) {
    worldTransform_.Initialize();
    worldTransform_.translation_ = pos;
    worldTransform_.scale_ = { 0.7f, 0.7f, 0.7f };
    model_ = std::unique_ptr<Model>(Model::CreateFromOBJ("expOrb")); // 経験値オーブ用モデル
    expValue_ = expValue;

    // 乱数生成（少し跳ねる感じの初期速度）
    static std::mt19937 rng{ std::random_device{}() };
    static std::uniform_real_distribution<float> dist(-0.5f, 0.5f);

    velocity_ = {
        dist(rng) * 0.05f,
        0.05f,
        dist(rng) * 0.05f
    };

    audio_ = Audio::GetInstance();
	pickupSEHandle_ = audio_->LoadWave("Sounds/se_exp.wav");
}

void ExpOrb::Update(const Vector3& playerPos) {
    if (!active_) return;

    // プレイヤーとの距離
    float dx = playerPos.x - worldTransform_.translation_.x;
    float dz = playerPos.z - worldTransform_.translation_.z;
    float distSq = dx * dx + dz * dz;

    // 一定距離以内なら吸い寄せられる
    if (distSq < 70.0f) {
        float dist = std::sqrt(distSq);
        if (dist > 0.001f) {
            dx /= dist; dz /= dist;
            // プレイヤー方向へ加速
            velocity_.x += dx * 0.05f;
            velocity_.z += dz * 0.05f;
        }
    }

    // 移動
    worldTransform_.translation_.x += velocity_.x;
    worldTransform_.translation_.y += velocity_.y;
    worldTransform_.translation_.z += velocity_.z;

    // 徐々に減速
    velocity_.x *= 0.95f;
    velocity_.y *= 0.95f;
    velocity_.z *= 0.95f;

    // 拾われ判定（近距離）
    if (distSq < 4.0f) { // 半径2.0以内
        Audio::GetInstance()->PlayWave(pickupSEHandle_, false, 1.0f);
        active_ = false;
    }

    worldTransform_.UpdateMatrix();
}

void ExpOrb::Draw(Camera* camera) {
    if (!active_ || !model_) return;
    model_->Draw(worldTransform_, *camera);
}