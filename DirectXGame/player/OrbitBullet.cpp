#include "OrbitBullet.h"
#include "ModelCache.h"
using namespace KamataEngine;

void OrbitBullet::Initialize(const Vector3& center, float radius, float angle) {
    Bullet::Initialize(center);

    orbitRadius_ = radius;
    angle_ = angle;

    model_ = ModelCache::Get("Bullet");

    worldTransform_.translation_ = {
        center.x + std::cos(angle_) * orbitRadius_,
        center.y,
        center.z + std::sin(angle_) * orbitRadius_
    };

    worldTransform_.UpdateMatrix();
}

void OrbitBullet::Update(const Vector3& center) {
    if (!active_) return;

    angle_ += angularSpeed_;

    worldTransform_.translation_ = {
        center.x + std::cos(angle_) * orbitRadius_,
        center.y,
        center.z + std::sin(angle_) * orbitRadius_
    };

    const float dt = 0.016f;
    for (auto it = hitCooldowns_.begin(); it != hitCooldowns_.end();) {
        it->second -= dt;
        if (it->second <= 0.0f) it = hitCooldowns_.erase(it);
        else ++it;
    }

    worldTransform_.UpdateMatrix();
}

void OrbitBullet::Draw(Camera* camera) {
    Bullet::Draw(camera);
}

bool OrbitBullet::CanHitEnemy(void* enemyPtr) {
    auto it = hitCooldowns_.find(enemyPtr);
    if (it == hitCooldowns_.end()) return true;
    return (it->second <= 0.0f);
}

void OrbitBullet::RegisterHit(void* enemyPtr) {
    hitCooldowns_[enemyPtr] = kHitInterval;
}
