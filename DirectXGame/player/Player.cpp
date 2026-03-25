#include "Player.h"
using namespace KamataEngine;

void Player::Initialize() {
    input_ = Input::GetInstance();
    camera_.Initialize();

    playerModel_ = std::unique_ptr<Model>(Model::CreateFromOBJ("octopus"));

    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };

    camera_.translation_ = { 0.0f, 80.0f, -45.0f };
    camera_.rotation_.x = 1.0f;
    camera_.UpdateMatrix();
}

void Player::Update() {
    UpdateMovement();
    UpdateCamera();
    worldTransform_.UpdateMatrix();
}

void Player::Draw() {
    if (visible_ && playerModel_) {
        playerModel_->Draw(worldTransform_, camera_);
    }
}

void Player::UpdateMovement() {
    const float kMoveSpeed = 0.5f;

    Vector3 move = { 0.0f, 0.0f, 0.0f };

    if (input_->PushKey(DIK_W)) move.z += kMoveSpeed;
    if (input_->PushKey(DIK_S)) move.z -= kMoveSpeed;
    if (input_->PushKey(DIK_A)) move.x -= kMoveSpeed;
    if (input_->PushKey(DIK_D)) move.x += kMoveSpeed;

    float moveLen = std::sqrt(move.x * move.x + move.z * move.z);
    if (moveLen > 0.0f) {
        // 正規化して一定速にする
        move.x = (move.x / moveLen) * kMoveSpeed;
        move.z = (move.z / moveLen) * kMoveSpeed;

        // 移動
        worldTransform_.translation_.x += move.x;
        worldTransform_.translation_.z += move.z;

        // ----- ここから回転をスムーズ化 -----
        float targetAngle = std::atan2(move.x, move.z); // 目標角度
        float currentAngle = worldTransform_.rotation_.y;

        // 角度差を -pi..pi に正規化
        constexpr float kPi = 3.14159265f;
        float diff = targetAngle - currentAngle;
        while (diff > kPi) diff -= 2.0f * kPi;
        while (diff < -kPi) diff += 2.0f * kPi;

        // 補間係数（0.0f: 回転しない, 1.0f: 即時回転）
        // 値を大きくすると回転が速く、値を小さくするとゆっくり滑らかになります。
        const float kRotateLerp = 0.50f;

        currentAngle += diff * kRotateLerp;
        // 必要なら currentAngle を -pi..pi に戻す（安定化）
        while (currentAngle > kPi) currentAngle -= 2.0f * kPi;
        while (currentAngle < -kPi) currentAngle += 2.0f * kPi;

        worldTransform_.rotation_.y = currentAngle;
        // ----- ここまで -----
    }
}

void Player::UpdateCamera() {
    camera_.translation_.x = worldTransform_.translation_.x;
    camera_.translation_.z = worldTransform_.translation_.z - 45.0f;
    camera_.rotation_.x = 1.0f;
    camera_.UpdateMatrix();
}
