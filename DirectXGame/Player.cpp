#include "Player.h"
using namespace KamataEngine;

Player::Player() {}

Player::~Player() {
    delete playerModel_;
    for (auto b : bullets_) {
        delete b;
    }
    bullets_.clear();
}

void Player::Initialize() {
    input_ = Input::GetInstance();
    camera_.Initialize();

    playerModel_ = Model::CreateFromOBJ("octopus");

    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
}

void Player::Update() {
    // プレイヤー位置固定（画面下中央）
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
    worldTransform_.rotation_.y = 0.0f;

    // 弾のクールタイム更新
    bulletTimer_ += 0.016f; // 1フレーム=0.016秒想定（60FPS）

    if (bulletTimer_ >= bulletCooldown_) {
        // 弾発射（正Z方向）
        Vector3 dir = { 0.0f, 0.0f, 1.0f };
        Bullet* bullet = new Bullet();
        bullet->Initialize(worldTransform_.translation_, dir, 0.5f);
        bullets_.push_back(bullet);

        bulletTimer_ = 0.0f;
    }

    // 弾の更新
    for (auto bullet : bullets_) {
        bullet->Update();
    }

    worldTransform_.UpdateMatrix();
}

void Player::Draw() {
    // プレイヤー描画
    playerModel_->Draw(worldTransform_, camera_);

    // 弾描画
    for (auto bullet : bullets_) {
        bullet->Draw();
    }
}