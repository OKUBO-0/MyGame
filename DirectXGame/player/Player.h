#pragma once

#include <KamataEngine.h>
#include <vector>
#include "Bullet.h"
#include "EnemyManager.h"

class EnemyManager;

class Player {
public:
    Player();
    ~Player();

    void Initialize();
    void Update();
    void Draw();

    // 状態取得
    KamataEngine::Vector3 GetWorldPosition() const { return worldTransform_.translation_; }
    const std::vector<Bullet*>& GetBullets() const { return bullets_; }
    KamataEngine::Camera& GetCamera() { return camera_; }

    // 外部連携
    void SetEnemyManager(EnemyManager* manager) { enemyManager_ = manager; }

    // ダメージ・状態判定
    void TakeDamage();
    bool IsInvincible() const { return invincible_; }
    bool IsDead() const { return lifeStock_ <= 0; }

    // EXP・レベル管理
    void AddEXP(int amount);
    int GetEXP() const { return exp_; }
    int GetLevel() const { return level_; }
    int GetNextLevelEXP() const { return nextLevelExp_; }
    bool IsLevelUpRequested() const { return levelUpRequested_; }
    void ClearLevelUpRequest() { levelUpRequested_ = false; }

    // アップグレード処理
    void UpgradeBulletPower();
    void UpgradeBulletCooldown();
    void RecoverHP();

    int GetBulletPower() const { return bulletPower_; }

    int GetHP() const { return lifeStock_; }
    int GetMaxHP() const { return maxLifeStock_; }

private:
    // エンジン関連
    KamataEngine::Input* input_ = nullptr;
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Camera camera_;
    KamataEngine::Model* playerModel_ = nullptr;

    // 弾管理
    std::vector<Bullet*> bullets_;
    float bulletCooldown_ = 1.0f;
    float bulletTimer_ = 0.0f;
    float range_ = 30.0f;
    int bulletPower_ = 1; // 🔼 弾の攻撃力

    // 敵連携
    EnemyManager* enemyManager_ = nullptr;

    // HP・無敵管理
    int lifeStock_ = 3;
    int maxLifeStock_ = 3;
    bool invincible_ = false;
    float invincibleTimer_ = 0.0f;
    bool visible_ = true;

    // EXP・レベルアップ管理
    int exp_ = 0;
    int level_ = 1;
    int nextLevelExp_ = 1;
    bool levelUpRequested_ = false;
};