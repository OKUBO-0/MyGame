#include "Enemy.h"
using namespace KamataEngine;

void Enemy::Initialize() {
    // ワールドトランスフォーム初期化（位置・回転・スケールの基準を設定）
    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f }; // 初期位置は原点

    // 敵をアクティブ状態に設定
    active_ = true;

    // オブジェクトカラー生成（デフォルトは白色）
    if (!objectColor_) {
        objectColor_ = std::make_unique<ObjectColor>();
        objectColor_->Initialize();
        objectColor_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    }

    // 白テクスチャをロード（ヒット時のフラッシュ用）
    whiteTextureHandle_ = TextureManager::Load("color/white.png");
}

void Enemy::SetPosition(const Vector3& pos) {
    worldTransform_.translation_ = pos;
}

void Enemy::SetPlayer(Player* player) {
    player_ = player;
}

void Enemy::SetModelByType(int32_t type) {
    enemyType_ = type; // ★ 行動タイプをセット

    switch (type) {
    case 0: enemyModel_.reset(Model::CreateFromOBJ("Enemy1")); break;
    case 1: enemyModel_.reset(Model::CreateFromOBJ("Enemy2")); break;
    case 2: enemyModel_.reset(Model::CreateFromOBJ("Enemy3")); break;
    case 3: enemyModel_.reset(Model::CreateFromOBJ("Enemy4")); break;
    default: enemyModel_.reset(Model::CreateFromOBJ("octopus")); break;
    }

    // ★ type1 は純粋に速度を上げる 
    if (type == 1) { 
        speedMultiplier_ = 2.0f; // 通常の2倍速 
    } else { 
        speedMultiplier_ = 1.0f; 
    }

    if (!objectColor_) {
        objectColor_ = std::make_unique<ObjectColor>();
        objectColor_->Initialize();
        objectColor_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    }
}

Vector3 Enemy::GetPosition() const {
    return worldTransform_.translation_;
}

bool Enemy::IsActive() const {
    return active_;
}

void Enemy::Deactivate() {
    active_ = false;
}

void Enemy::SetHP(int32_t hp) {
    hp_ = hp;
}

int32_t Enemy::GetHP() const {
    return hp_;
}

void Enemy::SetEXP(int32_t exp) {
    exp_ = exp;
}

int32_t Enemy::GetEXP() const {
    return exp_;
}

bool Enemy::JustDied() const {
    return justDied_;
}

void Enemy::ResetJustDied() {
    justDied_ = false;
}

void Enemy::Update() {
    if (!active_) return;

    const float kDeltaTime = 0.016f; // 60FPS前提

    // ヒット点滅処理
    if (hitFlashTimer_ > 0.0f) {
        hitFlashTimer_ -= kDeltaTime;
        if (hitFlashTimer_ <= 0.0f) {
            hitFlashTimer_ = 0.0f;
            if (objectColor_) objectColor_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        }
    }

    // ノックバック処理
    if (knockbackTimer_ > 0.0f) {
        worldTransform_.translation_.x += knockbackVelocity_.x;
        worldTransform_.translation_.z += knockbackVelocity_.z;

        knockbackVelocity_.x *= 0.88f;
        knockbackVelocity_.z *= 0.88f;

        knockbackTimer_ -= kDeltaTime;
        if (knockbackTimer_ <= 0.0f) {
            knockbackTimer_ = 0.0f;
            knockbackVelocity_ = { 0.0f, 0.0f, 0.0f };
        }
    }
    else {
        // プレイヤー追尾処理
        if (player_) {
            const Vector3& playerPos = player_->GetWorldPosition();
            Vector3 dir = {
                playerPos.x - worldTransform_.translation_.x,
                0.0f,
                playerPos.z - worldTransform_.translation_.z
            };

            float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
            if (len > 0.0f) {
                dir.x /= len;
                dir.z /= len;

                // 進行方向に応じてY軸回転を設定
                worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);

                float moveSpeed = speed_ * speedMultiplier_;

                // プレイヤーに向かって移動
                worldTransform_.translation_.x += dir.x * moveSpeed;
                worldTransform_.translation_.z += dir.z * moveSpeed;
            }
        }
    }

    // 行列更新
    worldTransform_.UpdateMatrix();
}

void Enemy::Draw(Camera* camera) {
    if (!active_ || !enemyModel_) return;

    // ヒット中は白テクスチャで上書き
    if (hitFlashTimer_ > 0.0f && whiteTextureHandle_ != 0 && objectColor_) {
        enemyModel_->Draw(worldTransform_, *camera, whiteTextureHandle_, objectColor_.get());
    }
    else {
        if (objectColor_) {
            enemyModel_->Draw(worldTransform_, *camera, objectColor_.get());
        }
        else {
            enemyModel_->Draw(worldTransform_, *camera);
        }
    }
}

void Enemy::TakeDamage(int32_t damage, const Vector3& knockDir, float strength) {
    // HP減少
    hp_ -= damage;

    // HPが0以下なら死亡処理
    if (hp_ <= 0) {
        Deactivate();
        justDied_ = true;
        return;
    }

    // 白フラッシュ開始
    hitFlashTimer_ = kHitFlashDuration;
    if (objectColor_) {
        objectColor_->SetColor({ 10.0f, 10.0f, 10.0f, 1.0f });
    }

    // ノックバック適用
    float len = std::sqrt(knockDir.x * knockDir.x + knockDir.z * knockDir.z);
    if (len > 0.0001f && strength > 0.0f) {
        Vector3 dir = knockDir;
        dir.x /= len;
        dir.z /= len;

        knockbackVelocity_.x = dir.x * strength;
        knockbackVelocity_.z = dir.z * strength;
        knockbackTimer_ = kKnockbackDuration;
    }
}