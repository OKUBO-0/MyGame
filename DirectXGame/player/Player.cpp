#include "Player.h"
using namespace KamataEngine;

Player::Player() {}

Player::~Player() {
    /// <summary>
    /// プレイヤーモデルを解放
    /// </summary>
    delete playerModel_;

    /// <summary>
    /// 弾インスタンスを解放
    /// </summary>
    for (auto b : bullets_) { delete b; }
    bullets_.clear();

    /// <summary>
    /// パーティクルインスタンスを解放
    /// </summary>
    for (auto p : effects_) { delete p; }
    effects_.clear();
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

    exp_ = 0;
    totalExp_ = 0;
}

void Player::Update() {
    const float kMoveSpeed = 0.2f;
    const float kDeltaTime = 0.016f;

    Vector3 move = { 0.0f, 0.0f, 0.0f };

    if (input_->PushKey(DIK_W)) { move.z += kMoveSpeed; }
    if (input_->PushKey(DIK_S)) { move.z -= kMoveSpeed; }
    if (input_->PushKey(DIK_A)) { move.x -= kMoveSpeed; }
    if (input_->PushKey(DIK_D)) { move.x += kMoveSpeed; }

    float moveLen = std::sqrt(move.x * move.x + move.z * move.z);
    if (moveLen > 0.0f) {
        move.x = (move.x / moveLen) * kMoveSpeed;
        move.z = (move.z / moveLen) * kMoveSpeed;

        worldTransform_.translation_.x += move.x;
        worldTransform_.translation_.z += move.z;

        worldTransform_.rotation_.y = std::atan2(move.x, move.z);

        effectTimer_ += kDeltaTime;
        if (effectTimer_ >= kEffectInterval) {
            RippleEffect* e = new RippleEffect();
            e->Initialize(worldTransform_.translation_);
            effects_.push_back(e);
            effectTimer_ = 0.0f;
        }
    }

    bulletTimer_ += kDeltaTime;

    if (invincible_) {
        invincibleTimer_ -= kDeltaTime;
        if (invincibleTimer_ <= 0.0f) {
            invincible_ = false;
            visible_ = true;
        }
        else {
            int32_t blinkFrame = static_cast<int32_t>(invincibleTimer_ * 10.0f);
            visible_ = (blinkFrame % 2 == 0);
        }
    }

    Vector3 nearestDir = { 0.0f, 0.0f, 1.0f };
    float minDistSq = FLT_MAX;
    bool enemyInRange = false;

    if (enemyManager_) {
        for (auto enemy : enemyManager_->GetEnemies()) {
            if (!enemy->IsActive()) { continue; }

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
        bullet->Initialize(worldTransform_.translation_, nearestDir, 0.5f);
        bullet->SetDamage(bulletPower_);
        bullets_.push_back(bullet);
        bulletTimer_ = 0.0f;
    }

    for (auto it = bullets_.begin(); it != bullets_.end();) {
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

    for (auto it = effects_.begin(); it != effects_.end();) {
        RippleEffect* e = *it;
        e->Update();
        if (!e->IsActive()) {
            delete e;
            it = effects_.erase(it);
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

    for (auto e : effects_) {
        e->Draw(&camera_);
    }
}

void Player::TakeDamage() {
    if (invincible_) { return; }

    lifeStock_--;
    invincible_ = true;
    invincibleTimer_ = 1.0f; ///< 無敵時間を定数化するなら kInvincibleDuration に変更可能
    visible_ = false;
}

void Player::AddEXP(int32_t amount) {
    exp_ += amount;
    totalExp_ += amount;
    while (exp_ >= nextLevelExp_) {
        exp_ -= nextLevelExp_;
        level_++;
        nextLevelExp_ = static_cast<int32_t>(nextLevelExp_ * 1.5f);
        levelUpRequested_ = true;
    }
}

void Player::UpgradeBulletPower() {
    bulletPower_ += 1;
}

void Player::UpgradeBulletCooldown() {
    bulletCooldown_ -= 0.1f;
}

void Player::RecoverHP() {
    lifeStock_ += 1;
    if (lifeStock_ > maxLifeStock_) {
        lifeStock_ = maxLifeStock_;
    }
}