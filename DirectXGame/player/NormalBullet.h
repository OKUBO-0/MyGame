#pragma once
#include "Bullet.h"
#include <unordered_map>

class NormalBullet : public Bullet {
public:
    // startPos → 弾の発射位置
    // targetPos → 狙う敵の位置
    void Initialize(const KamataEngine::Vector3& startPos,
        const KamataEngine::Vector3& targetPos,
        int power);

    void Update(const KamataEngine::Vector3& playerPos) override;
    void Draw(KamataEngine::Camera* camera) override;

    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }
    int32_t GetDamage() const { return power_; }

    // ヒット管理（OrbitBullet と同じ）
    bool CanHitEnemy(void* enemyPtr);
    void RegisterHit(void* enemyPtr);

    /// <summary>
    /// 弾を非アクティブ状態にする
    /// </summary>
    void Deactivate() { active_ = false; }

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