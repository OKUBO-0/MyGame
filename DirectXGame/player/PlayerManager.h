#pragma once

#include <KamataEngine.h>
#include <vector>
#include <memory>
#include <cstdint>
#include <string>

#include "Player.h"
#include "RippleEffect.h"
#include "NormalBullet.h"
#include "OrbitBullet.h"
#include "Drone.h"

class EnemyManager;

class PlayerManager {
public:
    void Initialize(Player* player);
    void LoadStatusFromCSV(const std::string& filePath);

    void Update();
    void Draw(KamataEngine::Camera* camera);

    // HP管理
    void TakeDamage();
    void RecoverHP();
    int32_t GetHP() const { return lifeStock_; }
    int32_t GetMaxHP() const { return maxLifeStock_; }
    bool IsInvincible() const { return invincible_; }
    bool IsDead() const { return lifeStock_ <= 0; }

    // EXP / レベル
    void AddEXP(int32_t amount);
    int32_t GetEXP() const { return exp_; }
    int32_t GetTotalEXP() const { return totalExp_; }
    int32_t GetLevel() const { return level_; }
    int32_t GetNextLevelEXP() const { return nextLevelExp_; }
    bool IsLevelUpRequested() const { return levelUpRequested_; }
    void ClearLevelUpRequest() { levelUpRequested_ = false; }

    // 攻撃力
    int32_t GetAttackPower() const { return attackPower_; }
    void UpgradeAttackPower() { attackPower_++; }

    // 通常弾
    void UpgradeNormalBullets();
    const std::vector<std::unique_ptr<NormalBullet>>& GetNormalBullets() const { return normalBullets_; }

    // 周囲弾
    void AddOrbitBullets();
    void UpgradeOrbitBullets();
    bool HasOrbitBullets() const { return hasOrbitBullets_; }
    const std::vector<std::unique_ptr<OrbitBullet>>& GetOrbitBullets() const { return orbitBullets_; }

    // ドローン
    void AddDrone();
    void UpgradeDrone();
    bool HasDrone() const { return hasDrone_; }
    const std::unique_ptr<Drone>& GetDrone() const { return drone_; }

    void SetEnemyManager(EnemyManager* manager) { enemyManager_ = manager; }

private:
    Player* player_ = nullptr;
    EnemyManager* enemyManager_ = nullptr;

    // HP管理
    bool invincible_ = false;
    float invincibleTimer_ = 0.0f;
    bool visible_ = true;

    // EXP / レベル
    int32_t level_;
    int32_t nextLevelExp_ ;
    int32_t maxLifeStock_ ;
    int32_t lifeStock_;
    int32_t exp_;
    int32_t totalExp_;
    int32_t attackPower_;
    bool levelUpRequested_ = false;

    // エフェクト
    std::vector<std::unique_ptr<RippleEffect>> effects_;
    float effectTimer_ = 0.0f;
    static constexpr float kEffectInterval = 0.2f;

    // 通常弾
    std::vector<std::unique_ptr<NormalBullet>> normalBullets_;
    bool hasNormalBullets_ = true;
    float normalBulletInterval_ = 1.0f;
    float normalBulletTimer_ = 0.0f;

    // 周囲弾
    std::vector<std::unique_ptr<OrbitBullet>> orbitBullets_;
    bool hasOrbitBullets_ = false;

    // ドローン
    std::unique_ptr<Drone> drone_;
    bool hasDrone_ = false;
    float droneInterval_ = 2.0f;
    float droneTimer_ = 0.0f;

    void UpdateInvincibility();
    void UpdateNormalBullets();
    void UpdateOrbitBullets();
    void UpdateDrone();
    void UpdateEffects();
};