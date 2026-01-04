#pragma once

#include <KamataEngine.h>
#include <vector>
#include <memory>
#include <cstdint>
#include "EnemyManager.h"
#include "RippleEffect.h"
#include "OrbitBullet.h"
#include "Bullet.h"

class EnemyManager;

/// <summary>
/// プレイヤーキャラクターを表すクラス。
/// 入力処理、弾の発射、HPやEXPの管理、敵との連携を行う。
/// </summary>
class Player {
public:
    void Initialize();
    void Update();
    void Draw();

    KamataEngine::Vector3 GetWorldPosition() const { return worldTransform_.translation_; }

    // --- 周囲弾のゲッター ---
    const std::vector<std::unique_ptr<OrbitBullet>>& GetOrbitBullets() const { return orbitBullets_; }

    KamataEngine::Camera& GetCamera() { return camera_; }
    void SetEnemyManager(EnemyManager* manager) { enemyManager_ = manager; }

    void TakeDamage();
    bool IsInvincible() const { return invincible_; }
    bool IsDead() const { return lifeStock_ <= 0; }

    void AddEXP(int32_t amount);
    int32_t GetEXP() const { return exp_; }
    int32_t GetTotalEXP() const { return totalExp_; }
    int32_t GetLevel() const { return level_; }
    int32_t GetNextLevelEXP() const { return nextLevelExp_; }
    bool IsLevelUpRequested() const { return levelUpRequested_; }
    void ClearLevelUpRequest() { levelUpRequested_ = false; }

    // HP回復
    void RecoverHP();

    // 周囲弾
    void AddOrbitBullets();
    void UpgradeOrbitBullets();
    bool HasOrbitBullets() const { return hasOrbitBullets_; }

    int32_t GetHP() const { return lifeStock_; }
    int32_t GetMaxHP() const { return maxLifeStock_; }

    // --- 通常弾 ---
    std::vector<std::unique_ptr<Bullet>> bullets_;
    bool hasNormalBullet_ = false;
    float bulletCooldown_ = 1.0f;
    float bulletTimer_ = 0.0f;
    int32_t bulletPower_ = 1;

    void AddNormalBullet(const KamataEngine::Vector3& dir);
    void UpgradeNormalBullet();
    const std::vector<std::unique_ptr<Bullet>>& GetBullets() const { return bullets_; }
    bool HasNormalBullet() const { return hasNormalBullet_; }

private:
    KamataEngine::Input* input_ = nullptr;
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Camera camera_;
    std::unique_ptr<KamataEngine::Model> playerModel_;

    EnemyManager* enemyManager_ = nullptr;

    int32_t lifeStock_ = 3;
    int32_t maxLifeStock_ = 3;
    bool invincible_ = false;
    float invincibleTimer_ = 0.0f;
    bool visible_ = true;

    int32_t exp_ = 0;
    int32_t totalExp_ = 0;
    int32_t level_ = 1;
    int32_t nextLevelExp_ = 1;
    bool levelUpRequested_ = false;

    std::vector<std::unique_ptr<RippleEffect>> effects_;
    float effectTimer_ = 0.0f;
    static constexpr float kEffectInterval = 0.2f;

    float range_ = 30.0f;

    // --- 周囲弾 ---
    std::vector<std::unique_ptr<OrbitBullet>> orbitBullets_;
    bool hasOrbitBullets_ = false;
};