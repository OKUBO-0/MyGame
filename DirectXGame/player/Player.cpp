#include "Player.h"
using namespace KamataEngine;

Player::Player() {}
Player::~Player() {
    delete playerModel_;
    for (auto b : bullets_) delete b;
    bullets_.clear();
}

void Player::Initialize() {
    input_ = Input::GetInstance();
    camera_.Initialize();

    playerModel_ = Model::CreateFromOBJ("octopus");

    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };

    level_ = 1;
    nextLevelExp_ = 50;
    bulletPower_ = 1;
    bulletCooldown_ = 1.0f;
    maxLifeStock_ = 3;
}

void Player::Update() {
    // 入力による移動
    const float moveSpeed = 0.2f;
    Vector3 move = { 0.0f, 0.0f, 0.0f };

    if (input_->PushKey(DIK_W)) move.z += moveSpeed;
    if (input_->PushKey(DIK_S)) move.z -= moveSpeed;
    if (input_->PushKey(DIK_A)) move.x -= moveSpeed;
    if (input_->PushKey(DIK_D)) move.x += moveSpeed;

    worldTransform_.translation_.x += move.x;
    worldTransform_.translation_.z += move.z;

    if (move.x != 0.0f || move.z != 0.0f) {
        worldTransform_.rotation_.y = std::atan2(move.x, move.z);
    }

    bulletTimer_ += 0.016f;

    // 無敵時間と点滅処理
    if (invincible_) {
        invincibleTimer_ -= 0.016f;
        if (invincibleTimer_ <= 0.0f) {
            invincible_ = false;
            visible_ = true;
        }
        else {
            int blinkFrame = static_cast<int>(invincibleTimer_ * 10.0f);
            visible_ = (blinkFrame % 2 == 0);
        }
    }

    // 射程内の敵を探索
    Vector3 nearestDir = { 0.0f, 0.0f, 1.0f };
    float minDistSq = FLT_MAX;
    bool enemyInRange = false;

    if (enemyManager_) {
        for (auto enemy : enemyManager_->GetEnemies()) {
            if (!enemy->IsActive()) continue;

            Vector3 ePos = enemy->GetPosition();
            Vector3 pPos = worldTransform_.translation_;
            float dx = ePos.x - pPos.x;
            float dz = ePos.z - pPos.z;
            float distSq = dx * dx + dz * dz;

            if (distSq <= range_ * range_) {
                enemyInRange = true;
                if (distSq < minDistSq) {
                    minDistSq = distSq;
                    nearestDir = { dx, 0.0f, dz };
                }
            }
        }
    }

    if (enemyInRange) {
        float len = std::sqrt(nearestDir.x * nearestDir.x + nearestDir.z * nearestDir.z);
        if (len > 0.0f) {
            nearestDir.x /= len;
            nearestDir.z /= len;
            worldTransform_.rotation_.y = std::atan2(nearestDir.x, nearestDir.z);
        }
    }
    else if (move.x != 0.0f || move.z != 0.0f) {
        worldTransform_.rotation_.y = std::atan2(move.x, move.z);
    }

    if (bulletTimer_ >= bulletCooldown_ && enemyInRange) {
        float len = std::sqrt(nearestDir.x * nearestDir.x + nearestDir.z * nearestDir.z);
        if (len > 0.0f) {
            nearestDir.x /= len;
            nearestDir.z /= len;
        }

        Bullet* bullet = new Bullet();
        bullet->Initialize(worldTransform_.translation_, nearestDir, 0.5f); // ✅ 弾速は固定
        bullet->SetDamage(bulletPower_); // ✅ 攻撃力を設定
        bullets_.push_back(bullet);
        bulletTimer_ = 0.0f;
    }

    // 弾更新と破棄
    for (auto it = bullets_.begin(); it != bullets_.end(); ) {
        Bullet* bullet = *it;
        bullet->Update(worldTransform_.translation_);
        if (!bullet->IsActive()) {
            delete bullet;
            it = bullets_.erase(it);
        }
        else {
            ++it;
        }
    }

    camera_.translation_.x = worldTransform_.translation_.x;
    camera_.translation_.z = worldTransform_.translation_.z;
    camera_.UpdateMatrix();

    worldTransform_.UpdateMatrix();
}

void Player::Draw() {
    if (visible_) {
        playerModel_->Draw(worldTransform_, camera_);
    }

    for (auto bullet : bullets_) {
        bullet->Draw(&camera_);
    }
}

void Player::TakeDamage() {
    if (invincible_) return;

    lifeStock_--;
    invincible_ = true;
    invincibleTimer_ = 1.0f;
    visible_ = false;
}

// ✅ レベルアップ処理
void Player::AddEXP(int amount) {
    exp_ += amount;
    while (exp_ >= nextLevelExp_) {
        exp_ -= nextLevelExp_;
        level_++;
        nextLevelExp_ = static_cast<int>(nextLevelExp_ * 1.5f);
        levelUpRequested_ = true;
    }
}

void Player::UpgradeBulletPower() {
    bulletPower_ += 1;
}

void Player::UpgradeBulletCooldown() {
    bulletCooldown_ -= 0.05f;
}

void Player::RecoverHP() {
    lifeStock_ += 1;
    if (lifeStock_ > maxLifeStock_) {
        lifeStock_ = maxLifeStock_;
    }
}