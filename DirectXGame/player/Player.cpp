#include "Player.h"
using namespace KamataEngine;

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

    AddOrbitBullets();
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

        // --- 移動時のエフェクト ---
        effectTimer_ += kDeltaTime;
        if (effectTimer_ >= kEffectInterval) {
            auto e = std::make_unique<RippleEffect>();
            e->Initialize(worldTransform_.translation_);
            effects_.push_back(std::move(e));
            effectTimer_ = 0.0f;
        }
    }

    // --- 無敵処理 ---
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

    // --- 周囲弾更新 ---
    if (hasOrbitBullets_) {
        for (auto& orb : orbitBullets_) {
            orb->Update(worldTransform_.translation_);
        }
    }

    // --- 通常弾発射処理（超スッキリ版） ---
    bulletTimer_ += kDeltaTime;

    if (hasNormalBullet_ && bulletTimer_ >= bulletCooldown_) {

        // ★ Bullet の射程を参照
        float attackRange = bullets_.empty() ? 30.0f : bullets_.front()->GetRange();

        Vector3 dir;

        // ★ Bullet に「射程内の敵探索＋方向計算」を任せる
        if (Bullet::GetDirectionToNearestEnemy(enemyManager_, worldTransform_.translation_, attackRange, dir)) {
            AddNormalBullet(dir);
            bulletTimer_ = 0.0f;
        }
    }

    // --- 弾更新 ---
    for (auto& b : bullets_) {
        b->Update(worldTransform_.translation_);
    }
    bullets_.erase(
        std::remove_if(bullets_.begin(), bullets_.end(),
            [](const std::unique_ptr<Bullet>& b) { return !b->IsActive(); }),
        bullets_.end()
    );

    // --- エフェクト更新 ---
    for (auto it = effects_.begin(); it != effects_.end();) {
        (*it)->Update();
        if (!(*it)->IsActive()) {
            it = effects_.erase(it);
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
    if (visible_ && playerModel_) {
        playerModel_->Draw(worldTransform_, camera_);
    }

    for (auto& e : effects_) {
        e->Draw(&camera_);
    }

    if (hasNormalBullet_) {
        for (auto& b : bullets_) {
            b->Draw(&camera_);
        }
    }

    if (hasOrbitBullets_) {
        for (auto& orb : orbitBullets_) {
            orb->Draw(&camera_);
        }
    }
}

void Player::TakeDamage() {
    if (invincible_) return;

    lifeStock_--;
    invincible_ = true;
    invincibleTimer_ = 1.0f;
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

void Player::AddNormalBullet(const Vector3& dir) {
    hasNormalBullet_ = true;
    auto b = std::make_unique<Bullet>();
    b->Initialize(worldTransform_.translation_, dir, bulletPower_);
    bullets_.push_back(std::move(b));
}

void Player::UpgradeNormalBullet() {
    bulletPower_++;
    bulletCooldown_ -= 0.2f;
    if (bulletCooldown_ < 0.2f) bulletCooldown_ = 0.2f;
}

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

void Player::UpgradeOrbitBullets() {
    orbitBulletPower_++;

    int newCount = static_cast<int>(orbitBullets_.size()) + 1;

    std::vector<std::unique_ptr<OrbitBullet>> newOrbs;
    for (int i = 0; i < newCount; ++i) {
        float angle = (2.0f * 3.14159265f * i) / newCount;
        auto orb = std::make_unique<OrbitBullet>();
        orb->Initialize(worldTransform_.translation_, 10.0f, angle, orbitBulletPower_);
        newOrbs.push_back(std::move(orb));
    }

    orbitBullets_ = std::move(newOrbs);
}

void Player::RecoverHP() {
    lifeStock_ += 1;
    if (lifeStock_ > maxLifeStock_) {
        lifeStock_ = maxLifeStock_;
    }
}