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

AABB Enemy::GetAABB() const {
    float halfSize = 1.0f; // モデルサイズに合わせて変更
    Vector3 pos = worldTransform_.translation_;
    AABB box;
    box.min = { pos.x - halfSize, pos.y - halfSize, pos.z - halfSize };
    box.max = { pos.x + halfSize, pos.y + halfSize, pos.z + halfSize };
    return box;
}