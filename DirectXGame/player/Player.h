#pragma once

#include <KamataEngine.h>
#include <vector>
#include <memory>
#include <cstdint>
#include "Bullet.h"
#include "EnemyManager.h"
#include "RippleEffect.h"
#include "OrbitBullet.h"
#include "DamageField.h"

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
    const std::vector<std::unique_ptr<Bullet>>& GetBullets() const { return bullets_; }
    // --- 周囲弾のゲッター ---
    const std::vector<std::unique_ptr<OrbitBullet>>& GetOrbitBullets() const { return orbitBullets_; }

    // --- ダメージフィールドのゲッター ---
    DamageField* GetDamageField() const { return damageField_.get(); }

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

    // 通常弾強化
    void UpgradeNormalBullet();

    // HP回復
    void RecoverHP();

    // 周囲弾
    void AddOrbitBullets();
    void UpgradeOrbitBullets();
    bool HasOrbitBullets() const { return hasOrbitBullets_; }

    // ダメージフィールド
    void AddDamageField();
    void UpgradeDamageField();
    bool HasDamageField() const { return hasDamageField_; }

    int32_t GetBulletPower() const { return bulletPower_; }
    int32_t GetHP() const { return lifeStock_; }
    int32_t GetMaxHP() const { return maxLifeStock_; }

private:
    KamataEngine::Input* input_ = nullptr;
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Camera camera_;
    std::unique_ptr<KamataEngine::Model> playerModel_;

    std::vector<std::unique_ptr<Bullet>> bullets_;
    float bulletCooldown_ = 1.0f;
    float bulletTimer_ = 0.0f;
    float range_ = 30.0f;
    int32_t bulletPower_ = 1;

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

    // --- 新しい攻撃方法 ---
    std::vector<std::unique_ptr<OrbitBullet>> orbitBullets_;
    bool hasOrbitBullets_ = false;

    std::unique_ptr<DamageField> damageField_;
    bool hasDamageField_ = false;
};