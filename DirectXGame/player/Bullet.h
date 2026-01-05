#pragma once
#include <KamataEngine.h>
#include <memory>
#include "EnemyManager.h"

class EnemyManager;

class Bullet {
public:
    void Initialize(const KamataEngine::Vector3& startPos,
        const KamataEngine::Vector3& direction,
        int32_t damage);

    void Update(const KamataEngine::Vector3& playerPos);
    void Draw(KamataEngine::Camera* camera);

    bool IsActive() const { return active_; }
    void Deactivate() { active_ = false; }

    int32_t GetDamage() const { return damage_; }
    float GetRange() const { return range_; }
    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

    void UpgradeDamage() { damage_++; }

    // ★ Player が使う「最も近い敵の方向を取得する」関数
    static bool GetDirectionToNearestEnemy(
        EnemyManager* enemyManager,
        const KamataEngine::Vector3& playerPos,
        float range,
        KamataEngine::Vector3& outDir);

private:
    KamataEngine::WorldTransform worldTransform_;
    std::unique_ptr<KamataEngine::Model> model_;

    KamataEngine::Vector3 direction_{ 0,0,0 };
    float speed_ = 0.6f;
    float range_ = 30.0f;
    int32_t damage_ = 1;
    bool active_ = true;
};