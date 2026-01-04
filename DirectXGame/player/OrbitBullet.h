#pragma once
#include <KamataEngine.h>
#include <memory>

/// <summary>
/// プレイヤーの周囲を回り続ける弾
/// </summary>
class OrbitBullet {
public:
    void Initialize(const KamataEngine::Vector3& center, float radius, float angle, int32_t damage);
    void Update(const KamataEngine::Vector3& center);
    void Draw(KamataEngine::Camera* camera);

    bool IsActive() const { return active_; }
    int32_t GetDamage() const { return damage_; }
    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

    void UpgradeDamage() { damage_++; }

    bool CanHitEnemy(void* enemyPtr); // 敵に当たれるか判定
    void RegisterHit(void* enemyPtr); // ヒット登録

private:
    KamataEngine::WorldTransform worldTransform_;
    std::unique_ptr<KamataEngine::Model> model_;

    float orbitRadius_ = 10.0f;   ///< プレイヤーからの半径
    float angle_ = 0.0f;         ///< 現在の角度
    float angularSpeed_ = 0.05f; ///< 回転速度
    int32_t damage_ = 1;         ///< 攻撃力
    bool active_ = true;

    std::unordered_map<void*, float> hitCooldowns_;
    static constexpr float kHitInterval = 0.5f; // 0.5秒は再ヒット禁止
};