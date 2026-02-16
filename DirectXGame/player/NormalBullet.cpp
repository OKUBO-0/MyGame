#include "NormalBullet.h"
using namespace KamataEngine;

void NormalBullet::InitializeForward(const Vector3& startPos,
    const Vector3& forward)
{
    Bullet::Initialize(startPos);
    audio_ = Audio::GetInstance();

    if (shotSEHandle_ == 0) {
        shotSEHandle_ = Audio::GetInstance()->LoadWave("Sounds/se_shot.wav");
    }

    Audio::GetInstance()->PlayWave(shotSEHandle_, false, 1.0f);

    model_ = std::unique_ptr<Model>(Model::CreateFromOBJ("Bullet"));

    direction_ = forward;

    float len = std::sqrt(direction_.x * direction_.x +
        direction_.y * direction_.y +
        direction_.z * direction_.z);

    if (len > 0.0001f) {
        direction_.x /= len;
        direction_.y /= len;
        direction_.z /= len;
    }

    traveled_ = 0.0f;
    worldTransform_.UpdateMatrix();
}

void NormalBullet::Update(const Vector3&) {
    if (!active_) return;

    worldTransform_.translation_.x += direction_.x * speed_;
    worldTransform_.translation_.y += direction_.y * speed_;
    worldTransform_.translation_.z += direction_.z * speed_;

    traveled_ += speed_;

    if (traveled_ >= range_) {
        active_ = false;
        return;
    }

    const float dt = 0.016f;
    for (auto it = hitCooldowns_.begin(); it != hitCooldowns_.end();) {
        it->second -= dt;
        if (it->second <= 0.0f) it = hitCooldowns_.erase(it);
        else ++it;
    }

    worldTransform_.UpdateMatrix();
}

void NormalBullet::Draw(Camera* camera) {
    Bullet::Draw(camera);
}

bool NormalBullet::CanHitEnemy(void* enemyPtr) {
    auto it = hitCooldowns_.find(enemyPtr);
    if (it == hitCooldowns_.end()) return true;
    return (it->second <= 0.0f);
}

void NormalBullet::RegisterHit(void* enemyPtr) {
    hitCooldowns_[enemyPtr] = kHitInterval;
}