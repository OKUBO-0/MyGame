#pragma once
#include <KamataEngine.h>
#include <memory>

/// <summary>
/// プレイヤー中心に展開されるダメージフィールド
/// </summary>
class DamageField {
public:
    void Initialize(float radius, int32_t damage);
    void Update(const KamataEngine::Vector3& center);
    void Draw(KamataEngine::Camera* camera);

    bool IsActive() const { return active_; }
    int32_t GetDamage() const { return damage_; }
    float GetRadius() const { return radius_; }

    void UpgradeDamage() { damage_++; }
    void IncreaseRadius(float delta) { radius_ += delta; }

private:
    KamataEngine::WorldTransform worldTransform_;
    std::unique_ptr<KamataEngine::Model> model_;

    float radius_ = 3.0f;   ///< ダメージ範囲半径
    int32_t damage_ = 1;    ///< ダメージ量
    bool active_ = true;
};