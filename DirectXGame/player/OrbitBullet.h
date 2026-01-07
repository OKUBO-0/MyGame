#pragma once
#include "Bullet.h"
#include <unordered_map>

/// <summary>
/// プレイヤーの周囲を回り続ける弾（Bullet継承 + 以前の機能統合）
/// </summary>
class OrbitBullet : public Bullet {
public:
    void Initialize(const KamataEngine::Vector3& center, float radius, float angle, int32_t damage);
    void Update(const KamataEngine::Vector3& center) override;
    void Draw(KamataEngine::Camera* camera) override;

    // 攻撃力（Bullet::power_ を使用）
    int32_t GetDamage() const { return power_; }
    void UpgradeDamage() { power_++; }

    // 敵へのヒット管理
    bool CanHitEnemy(void* enemyPtr);
    void RegisterHit(void* enemyPtr);

    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

private:
    float orbitRadius_ = 10.0f;   ///< プレイヤーからの半径
    float angle_ = 0.0f;          ///< 現在の角度
    float angularSpeed_ = 0.05f;  ///< 回転速度

    // 再ヒット禁止管理
    std::unordered_map<void*, float> hitCooldowns_;
    static constexpr float kHitInterval = 0.5f; // 0.5秒は再ヒット禁止
};