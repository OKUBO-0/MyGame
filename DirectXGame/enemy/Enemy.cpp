#include "Enemy.h"
using namespace KamataEngine;

void Enemy::Initialize() {
    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0,0,0 };
    active_ = true;

    if (!objectColor_) {
        objectColor_ = std::make_unique<ObjectColor>();
        objectColor_->Initialize();
        objectColor_->SetColor({ 1,1,1,1 });
    }

    whiteTextureHandle_ = TextureManager::Load("color/white.png");

    audio_ = Audio::GetInstance();
    deathSEHandle_ = audio_->LoadWave("Sounds/se_death.wav");
}

void Enemy::SetPosition(const Vector3& pos) {
    worldTransform_.translation_ = pos;
}

void Enemy::SetPlayer(Player* player) {
    player_ = player;
}

void Enemy::SetModelByType(int32_t type) {
    enemyType_ = type;

    switch (type) {
    case 0: enemyModel_.reset(Model::CreateFromOBJ("Enemy1")); break;
    case 1: enemyModel_.reset(Model::CreateFromOBJ("Enemy2")); break;
    case 2: enemyModel_.reset(Model::CreateFromOBJ("Enemy3")); break;
    case 3: enemyModel_.reset(Model::CreateFromOBJ("Enemy4")); break;
    default: enemyModel_.reset(Model::CreateFromOBJ("octopus")); break;
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

    const float dt = 0.016f;

    // --- ヒット点滅 ---
    if (hitFlashTimer_ > 0.0f) {
        hitFlashTimer_ -= dt;
        if (hitFlashTimer_ <= 0.0f && objectColor_) {
            objectColor_->SetColor({ 1,1,1,1 });
        }
    }

    // --- ノックバック ---
    if (knockbackTimer_ > 0.0f) {
        worldTransform_.translation_.x += knockbackVelocity_.x;
        worldTransform_.translation_.z += knockbackVelocity_.z;

        knockbackVelocity_.x *= 0.88f;
        knockbackVelocity_.z *= 0.88f;

        knockbackTimer_ -= dt;
        if (knockbackTimer_ <= 0.0f) {
            knockbackVelocity_ = { 0,0,0 };
        }
    }
    else {
        // --- 通常行動 ---
        if (enemyType_ == 2) {
            UpdateType2();
        }
        else if (player_) {
            Vector3 p = player_->GetWorldPosition();
            Vector3 dir = { p.x - worldTransform_.translation_.x, 0, p.z - worldTransform_.translation_.z };

            float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
            if (len > 0.0f) {
                dir.x /= len;
                dir.z /= len;

                worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);

                float moveSpeed = speed_;
                worldTransform_.translation_.x += dir.x * moveSpeed;
                worldTransform_.translation_.z += dir.z * moveSpeed;
            }
        }
    }

    worldTransform_.UpdateMatrix();
}

void Enemy::UpdateType2() {
    if (!player_) return;

    Vector3 p = player_->GetWorldPosition();
    Vector3 toPlayer = { p.x - worldTransform_.translation_.x, 0, p.z - worldTransform_.translation_.z };

    float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z);
    if (dist > 0.001f) {
        toPlayer.x /= dist;
        toPlayer.z /= dist;
    }

    Vector3 side = { -toPlayer.z, 0, toPlayer.x };

    Vector3 finalDir = {
        toPlayer.x * approachSpeed_ + side.x * circleSpeed_,
        0,
        toPlayer.z * approachSpeed_ + side.z * circleSpeed_
    };

    float len = std::sqrt(finalDir.x * finalDir.x + finalDir.z * finalDir.z);
    if (len > 0.001f) {
        finalDir.x /= len;
        finalDir.z /= len;
    }

    float moveSpeed = speed_;
    worldTransform_.translation_.x += finalDir.x * moveSpeed;
    worldTransform_.translation_.z += finalDir.z * moveSpeed;

    worldTransform_.rotation_.y = std::atan2(finalDir.x, finalDir.z);
}

void Enemy::Draw(Camera* camera) {
    if (!active_ || !enemyModel_) return;

    if (hitFlashTimer_ > 0.0f && whiteTextureHandle_ != 0 && objectColor_) {
        enemyModel_->Draw(worldTransform_, *camera, whiteTextureHandle_, objectColor_.get());
    }
    else {
        enemyModel_->Draw(worldTransform_, *camera, objectColor_.get());
    }
}

void Enemy::TakeDamage(int32_t damage, const Vector3& knockDir, float strength) {
    hp_ -= damage;

    if (hp_ <= 0) {
        audio_->PlayWave(deathSEHandle_, false, 1.0f);
        Deactivate();
        justDied_ = true;
        return;
    }

    hitFlashTimer_ = kHitFlashDuration;
    if (objectColor_) {
        objectColor_->SetColor({ 10,10,10,1 });
    }

    float len = std::sqrt(knockDir.x * knockDir.x + knockDir.z * knockDir.z);
    if (len > 0.001f && strength > 0.0f) {
        Vector3 dir = { knockDir.x / len, 0, knockDir.z / len };
        knockbackVelocity_.x = dir.x * strength;
        knockbackVelocity_.z = dir.z * strength;
        knockbackTimer_ = kKnockbackDuration;
    }
}