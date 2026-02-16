#pragma once
#include "Bullet.h"
#include <unordered_map>

class NormalBullet : public Bullet {
public:
    void InitializeForward(const KamataEngine::Vector3& startPos,
        const KamataEngine::Vector3& forward);

    void Update(const KamataEngine::Vector3& playerPos) override;
    void Draw(KamataEngine::Camera* camera) override;

    bool CanHitEnemy(void* enemyPtr);
    void RegisterHit(void* enemyPtr);

    void Deactivate() { active_ = false; }

    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

private:
    KamataEngine::Vector3 direction_{ 0,0,0 };
    float speed_ = 1.0f;
    float range_ = 30.0f;
    float traveled_ = 0.0f;

    std::unordered_map<void*, float> hitCooldowns_;
    static constexpr float kHitInterval = 0.5f;

    KamataEngine::Audio* audio_ = nullptr;
    uint32_t shotSEHandle_ = 0;
};