#pragma once

#include <KamataEngine.h>
#include <vector>
#include <memory>
#include <cstdint>

#include "EnemyManager.h"
#include "RippleEffect.h"
#include "NormalBullet.h"
#include "OrbitBullet.h"

class EnemyManager;

class Player {
public:
    void Initialize();
    void Update();
    void Draw();

    // 基本情報
    KamataEngine::Vector3 GetWorldPosition() const { return worldTransform_.translation_; }
    KamataEngine::Camera& GetCamera() { return camera_; }
    void SetEnemyManager(EnemyManager* manager) { enemyManager_ = manager; }

    // 状態
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

    // HP
    void TakeDamage();
    void RecoverHP();
    int32_t GetHP() const { return lifeStock_; }
    int32_t GetMaxHP() const { return maxLifeStock_; }

    void UpgradeNormalBullets();

    // 通常弾（NormalBullet）のリストを取得
    const std::vector<std::unique_ptr<NormalBullet>>& GetNormalBullets() const { return normalBullets_; }

    // 周囲弾を追加
    void AddOrbitBullets();

    // 周囲弾を強化（数を増やす）
    void UpgradeOrbitBullets();

    // 周囲弾を持っているか？
    bool HasOrbitBullets() const { return hasOrbitBullets_; }

    // 周囲弾の参照
    const std::vector<std::unique_ptr<OrbitBullet>>& GetOrbitBullets() const { return orbitBullets_; }

private:
    // 入力・描画
    KamataEngine::Input* input_ = nullptr;
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Camera camera_;
    std::unique_ptr<KamataEngine::Model> playerModel_;

    // 敵管理
    EnemyManager* enemyManager_ = nullptr;

    // HP
    int32_t lifeStock_ = 3;
    int32_t maxLifeStock_ = 3;
    bool invincible_ = false;
    float invincibleTimer_ = 0.0f;
    bool visible_ = true;

    // EXP / レベル
    int32_t exp_ = 0;
    int32_t totalExp_ = 0;
    int32_t level_ = 1;
    int32_t nextLevelExp_ = 1;
    bool levelUpRequested_ = false;

    // エフェクト
    std::vector<std::unique_ptr<RippleEffect>> effects_;
    float effectTimer_ = 0.0f;
    static constexpr float kEffectInterval = 0.2f;

    // 周囲弾のリスト
    std::vector<std::unique_ptr<OrbitBullet>> orbitBullets_;

    // 周囲弾を持っているか？
    bool hasOrbitBullets_ = false;

    // 周囲弾の攻撃力
    int32_t orbitBulletPower_ = 1;

    std::vector<std::unique_ptr<NormalBullet>> normalBullets_;
    bool hasNormalBullets_ = true;

    float normalBulletInterval_ = 1.0f;
    float normalBulletTimer_ = 0.0f;
    int32_t normalBulletPower_ = 1;
};