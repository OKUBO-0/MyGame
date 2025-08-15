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
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
}

void Enemy::Update() {

    // ワールドトランスフォームの更新
    worldTransform_.UpdateMatrix();
}

void Enemy::Draw() {
    // 描画
    enemyModel_->Draw(worldTransform_, camera_);
}