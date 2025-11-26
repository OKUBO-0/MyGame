#include "Enemy.h"
using namespace KamataEngine;

Enemy::Enemy() {}
Enemy::~Enemy() {
    // 敵モデルを動的に生成しているため、破棄時に解放
    delete enemyModel_;
}

void Enemy::Initialize() {
    // ワールドトランスフォームを初期化（位置・回転・スケールをリセット）
    worldTransform_.Initialize();

    // 初期位置を原点に設定
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };

    // 敵をアクティブ状態にする
    active_ = true;
}

void Enemy::SetModelByType(int type) {
    // 既存モデルがある場合は解放してから新しいモデルを設定
    if (enemyModel_) {
        delete enemyModel_;
        enemyModel_ = nullptr;
    }

    // 敵の種類に応じて異なるモデルを読み込む
    switch (type) {
    case 0:
        enemyModel_ = Model::CreateFromOBJ("Enemy1");
        break;
    case 1:
        enemyModel_ = Model::CreateFromOBJ("Enemy2");
        break;
    case 2:
        enemyModel_ = Model::CreateFromOBJ("Enemy3");
        break;
    case 3:
        enemyModel_ = Model::CreateFromOBJ("Enemy4");
        break;
    default:
        // デフォルトはタコモデル（octopus）
        enemyModel_ = Model::CreateFromOBJ("octopus");
        break;
    }
}

void Enemy::Update() {
    // 非アクティブ状態なら処理しない
    if (!active_) return;

    // プレイヤーが存在する場合のみ追尾処理を行う
    if (player_) {
        // プレイヤー位置との差分ベクトルを計算（Y方向は無視してXZ平面のみ）
        const Vector3& playerPos = player_->GetWorldPosition();
        Vector3 dir = {
            playerPos.x - worldTransform_.translation_.x,
            0.0f,
            playerPos.z - worldTransform_.translation_.z
        };

        // ベクトルの長さを計算
        float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);

        // プレイヤーが一定距離以上にいる場合のみ正規化して移動
        if (len > 0.0f) {
            dir.x /= len;
            dir.z /= len;

            // 敵の向きを移動方向に合わせる（Y軸回転）
            worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);

            // プレイヤー方向へ speed_ 分だけ移動
            worldTransform_.translation_.x += dir.x * speed_;
            worldTransform_.translation_.z += dir.z * speed_;
        }
    }

    // ワールド行列を更新（位置・回転・スケールを反映）
    worldTransform_.UpdateMatrix();
}

void Enemy::Draw(KamataEngine::Camera* camera) {
    // 非アクティブまたはモデル未設定なら描画しない
    if (!active_ || !enemyModel_) return;

    // 敵モデルを描画
    enemyModel_->Draw(worldTransform_, *camera);
}

void Enemy::TakeDamage(int damage) {
    // ダメージを受けてHPを減少
    hp_ -= damage;

    // HPが0以下になったら非アクティブ化
    if (hp_ <= 0) {
        Deactivate();
    }
}