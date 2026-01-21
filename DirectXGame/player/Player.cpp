#include "Player.h"
using namespace KamataEngine;

//==================================================
//  初期化
//==================================================
void Player::Initialize() {
    input_ = Input::GetInstance();
    camera_.Initialize();

    playerModel_ = std::unique_ptr<Model>(Model::CreateFromOBJ("octopus"));

    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };

    level_ = 1;
    nextLevelExp_ = 10;
    maxLifeStock_ = 3;
    exp_ = 0;
    totalExp_ = 0;
}

//==================================================
//  更新
//==================================================
void Player::Update() {
    const float kMoveSpeed = 0.2f;
    const float kDeltaTime = 0.016f;

    Vector3 move = { 0.0f, 0.0f, 0.0f };

    // -------------------------
    // 入力処理
    // -------------------------
    if (input_->PushKey(DIK_W)) move.z += kMoveSpeed;
    if (input_->PushKey(DIK_S)) move.z -= kMoveSpeed;
    if (input_->PushKey(DIK_A)) move.x -= kMoveSpeed;
    if (input_->PushKey(DIK_D)) move.x += kMoveSpeed;

    // -------------------------
    // 移動処理
    // -------------------------
    float moveLen = std::sqrt(move.x * move.x + move.z * move.z);
    if (moveLen > 0.0f) {
        move.x = (move.x / moveLen) * kMoveSpeed;
        move.z = (move.z / moveLen) * kMoveSpeed;

        worldTransform_.translation_.x += move.x;
        worldTransform_.translation_.z += move.z;
        worldTransform_.rotation_.y = std::atan2(move.x, move.z);

        // 移動エフェクト
        effectTimer_ += kDeltaTime;
        if (effectTimer_ >= kEffectInterval) {
            auto e = std::make_unique<RippleEffect>();
            e->Initialize(worldTransform_.translation_);
            effects_.push_back(std::move(e));
            effectTimer_ = 0.0f;
        }
    }

    // -------------------------
    // 無敵処理
    // -------------------------
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

    // -------------------------
    // 通常弾（NormalBullet）
    // -------------------------
    if (hasNormalBullets_) {
        normalBulletTimer_ += kDeltaTime;

        // 敵探索
        Enemy* nearest = nullptr;
        float nearestDistSq = 999999.0f;

        for (auto& e : enemyManager_->GetEnemies()) {
            if (!e->IsActive()) continue;

            Vector3 ePos = e->GetPosition();
            Vector3 pPos = worldTransform_.translation_;

            float dx = ePos.x - pPos.x;
            float dz = ePos.z - pPos.z;
            float distSq = dx * dx + dz * dz;

            if (distSq < nearestDistSq && distSq < 30.0f * 30.0f) {
                nearestDistSq = distSq;
                nearest = e.get();
            }
        }

        // 発射
        if (nearest && normalBulletTimer_ >= normalBulletInterval_) {
            auto b = std::make_unique<NormalBullet>();
            b->Initialize(worldTransform_.translation_, nearest->GetPosition(), normalBulletPower_);
            normalBullets_.push_back(std::move(b));
            normalBulletTimer_ = 0.0f;
        }

        // 更新
        for (auto& b : normalBullets_) {
            b->Update(worldTransform_.translation_);
        }

        // 削除
        normalBullets_.erase(
            std::remove_if(normalBullets_.begin(), normalBullets_.end(),
                [](const std::unique_ptr<NormalBullet>& b) { return !b->IsActive(); }),
            normalBullets_.end()
        );
    }

    // -------------------------
    // 周囲弾（OrbitBullet）
    // -------------------------
    if (hasOrbitBullets_) {
        for (auto& orb : orbitBullets_) {
            orb->Update(worldTransform_.translation_);
        }
    }

    // -------------------------
    // エフェクト更新
    // -------------------------
    for (auto it = effects_.begin(); it != effects_.end();) {
        (*it)->Update();
        if (!(*it)->IsActive()) {
            it = effects_.erase(it);
        }
        else {
            ++it;
        }
    }

    // -------------------------
    // カメラ追従
    // -------------------------
    camera_.translation_.x = worldTransform_.translation_.x;
    camera_.translation_.z = worldTransform_.translation_.z - 45.0f;
    camera_.rotation_.x = 1.0f;
    camera_.UpdateMatrix();

    // -------------------------
    // 行列更新
    // -------------------------
    worldTransform_.UpdateMatrix();
}

//==================================================
//  描画
//==================================================
void Player::Draw() {
    if (visible_ && playerModel_) {
        playerModel_->Draw(worldTransform_, camera_);
    }

    for (auto& e : effects_) {
        e->Draw(&camera_);
    }

    // 通常弾描画
    for (auto& b : normalBullets_) {
        b->Draw(&camera_);
    }

    // 周囲弾描画
    if (hasOrbitBullets_) {
        for (auto& orb : orbitBullets_) {
            orb->Draw(&camera_);
        }
    }
}

//==================================================
//  ダメージ処理
//==================================================
void Player::TakeDamage() {
    if (invincible_) return;

    lifeStock_--;
    invincible_ = true;
    invincibleTimer_ = 1.0f;
    visible_ = false;
}

//==================================================
//  経験値処理
//==================================================
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

//==================================================
//  HP回復
//==================================================
void Player::RecoverHP() {
    lifeStock_++;
    if (lifeStock_ > maxLifeStock_) {
        lifeStock_ = maxLifeStock_;
    }
}

void Player::UpgradeNormalBullets() {
    normalBulletPower_++;

    // 発射間隔短縮
    normalBulletInterval_ *= 0.5f;
}

//==================================================
//  周囲弾生成
//==================================================
void Player::AddOrbitBullets() {
    hasOrbitBullets_ = true;

    const int bulletCount = 1;
    orbitBullets_.clear();

    for (int i = 0; i < bulletCount; ++i) {
        float angle = (2.0f * 3.14159265f * i) / bulletCount;

        auto orb = std::make_unique<OrbitBullet>();
        orb->Initialize(worldTransform_.translation_, 10.0f, angle, orbitBulletPower_);

        orbitBullets_.push_back(std::move(orb));
    }
}

//==================================================
//  周囲弾強化（数を増やす）
//==================================================
void Player::UpgradeOrbitBullets() {
    orbitBulletPower_++;

    int newCount = static_cast<int>(orbitBullets_.size()) + 1;

    std::vector<std::unique_ptr<OrbitBullet>> newOrbs;
    newOrbs.reserve(newCount);

    for (int i = 0; i < newCount; ++i) {
        float angle = (2.0f * 3.14159265f * i) / newCount;

        auto orb = std::make_unique<OrbitBullet>();
        orb->Initialize(worldTransform_.translation_, 10.0f, angle, orbitBulletPower_);

        newOrbs.push_back(std::move(orb));
    }

    orbitBullets_ = std::move(newOrbs);
}