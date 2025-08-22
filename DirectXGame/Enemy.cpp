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
    // プレイヤーが設定されている場合のみ向きを合わせる
    if (player_) {
        // プレイヤーのワールド座標を取得
        Vector3 playerPos = player_->GetWorldPosition();

        // 敵のワールド座標
        Vector3 enemyPos = worldTransform_.translation_;

        // プレイヤー方向ベクトルを計算
        Vector3 toPlayer = {
            playerPos.x - enemyPos.x,
            0.0f, // Y軸回転だけしたいので高さ方向は無視
            playerPos.z - enemyPos.z
        };

        // 角度を求める（Z軸正方向を基準としたラジアン）
        float targetAngle = std::atan2(toPlayer.x, toPlayer.z);

        // 敵のY回転をプレイヤー方向に合わせる
        worldTransform_.rotation_.y = targetAngle;
    }

    // ワールド行列更新
    worldTransform_.UpdateMatrix();
}

void Enemy::Draw() {
    // 描画
    enemyModel_->Draw(worldTransform_, camera_);
}