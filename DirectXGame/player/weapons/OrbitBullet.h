#pragma once
#include "Bullet.h"
#include <unordered_map>

namespace DirectXGame {

class OrbitBullet : public Bullet {
public:
    void Initialize(const KamataEngine::Vector3& center, float radius, float angle, float angularSpeed);
    void Update(const KamataEngine::Vector3& center, float deltaTime) override;
    void Draw(KamataEngine::Camera* camera) override;

    bool CanHitEnemy(void* enemyPtr);
    void RegisterHit(void* enemyPtr);

    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

private:
    float orbitRadius_ = 10.0f;
    float angle_ = 0.0f;
    float angularSpeed_ = 0.05f;

    std::unordered_map<void*, float> hitCooldowns_;
    static constexpr float kHitInterval = 0.5f;
};

} // namespace DirectXGame
