#include "Enemy.h"

using namespace KamataEngine;

Enemy::Enemy() {
}

Enemy::~Enemy() {
    delete enemyModel_;
}

void Enemy::Initialize() {
    // カメラ初期化（敵用に個別カメラ）
    camera_.Initialize();

    // モデル読み込み（仮に "enemy" とする）
    enemyModel_ = Model::CreateFromOBJ("octopus");

    // ワールドトランスフォーム初期化
    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 5.0f };
}

void Enemy::Update() {
    if (player_) {
        // プレイヤー座標を取得
        const auto& playerPos = player_->GetWorldPosition();

        // プレイヤー方向ベクトル
        Vector3 dir = {
            playerPos.x - worldTransform_.translation_.x,
            0.0f,
            playerPos.z - worldTransform_.translation_.z
        };

        // 正規化
        float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
        if (len > 0.0f) {
            dir.x /= len;
            dir.z /= len;
        }

        // 敵の向きをプレイヤー方向へ
        worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);
    }

    // 行列更新
    worldTransform_.UpdateMatrix();
}

void Enemy::Draw() {
    // 描画
    enemyModel_->Draw(worldTransform_, camera_);
}