#include "Player.h"
using namespace KamataEngine;

void Player::Initialize() {
    // 入力・カメラ初期化
    input_ = Input::GetInstance();
    camera_.Initialize();

    // プレイヤーモデル生成（タコモデル）
    playerModel_ = std::unique_ptr<Model>(Model::CreateFromOBJ("octopus"));

    // ワールドトランスフォーム初期化（位置・回転・スケール）
    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };

    // 初期ステータス設定
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

    // --- 入力による移動処理 ---
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

        // --- 移動時のエフェクト生成 ---
        effectTimer_ += kDeltaTime;
        if (effectTimer_ >= kEffectInterval) {
            auto e = std::make_unique<RippleEffect>();
            e->Initialize(worldTransform_.translation_);
            effects_.push_back(std::move(e));
            effectTimer_ = 0.0f;
        }
    }

    bulletTimer_ += kDeltaTime;

    // --- 無敵状態の処理（点滅演出） ---
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

    // --- 敵探索（最も近い敵を狙う） ---
    Vector3 nearestDir = { 0.0f, 0.0f, 1.0f };
    float minDistSq = FLT_MAX;
    bool enemyInRange = false;

    if (enemyManager_) {
        for (auto& enemy : enemyManager_->GetEnemies()) {
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

    // --- 弾発射処理 ---
    if (bulletTimer_ >= bulletCooldown_ && enemyInRange) {
        float len = std::sqrt(nearestDir.x * nearestDir.x + nearestDir.z * nearestDir.z);
        if (len > 0.0f) {
            nearestDir.x /= len;
            nearestDir.z /= len;
        }

        auto bullet = std::make_unique<Bullet>();
        bullet->Initialize(worldTransform_.translation_, nearestDir, 0.5f);
        bullet->SetDamage(bulletPower_);
        bullets_.push_back(std::move(bullet));
        bulletTimer_ = 0.0f;
    }

    // --- 弾更新 ---
    for (auto it = bullets_.begin(); it != bullets_.end();) {
        (*it)->Update(worldTransform_.translation_);
        if (!(*it)->IsActive()) {
            it = bullets_.erase(it); // unique_ptr により自動解放
        }
        else {
            ++it;
        }
    }

    // --- エフェクト更新 ---
    for (auto it = effects_.begin(); it != effects_.end();) {
        (*it)->Update();
        if (!(*it)->IsActive()) {
            it = effects_.erase(it); // unique_ptr により自動解放
        }
        else {
            ++it;
        }
    }

    // --- カメラ追従 ---
    camera_.translation_.x = worldTransform_.translation_.x;
    camera_.translation_.z = worldTransform_.translation_.z;
    camera_.UpdateMatrix();

    // --- 行列更新 ---
    worldTransform_.UpdateMatrix();
}

void Player::Draw() {
    // --- プレイヤーモデル描画 ---
    if (visible_ && playerModel_) {
        playerModel_->Draw(worldTransform_, camera_);
    }

    // --- 弾描画 ---
    for (auto& bullet : bullets_) {
        bullet->Draw(&camera_);
    }

    // --- エフェクト描画 ---
    for (auto& e : effects_) {
        e->Draw(&camera_);
    }
}

void Player::TakeDamage() {
    if (invincible_) return;

    // --- ダメージ処理 ---
    lifeStock_--;
    invincible_ = true;
    invincibleTimer_ = 1.0f; // 無敵時間（定数化可能）
    visible_ = false;
}

void Player::AddEXP(int32_t amount) {
    // --- 経験値加算処理 ---
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
    bulletPower_ += 1; // 弾攻撃力強化
}

void Player::UpgradeBulletCooldown() {
    bulletCooldown_ -= 0.1f; // 弾発射間隔短縮
}

void Player::RecoverHP() {
    lifeStock_ += 1; // HP回復
    if (lifeStock_ > maxLifeStock_) {
        lifeStock_ = maxLifeStock_;
    }
}