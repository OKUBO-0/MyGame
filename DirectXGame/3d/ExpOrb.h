#pragma once
#include <KamataEngine.h>
#include <memory>

class ExpOrb {
public:
    void Initialize(const KamataEngine::Vector3& pos, int32_t expValue);
    void Update(const KamataEngine::Vector3& playerPos);
    void Draw(KamataEngine::Camera* camera);

    bool IsActive() const { return active_; }
    int32_t GetEXP() const { return expValue_; }
    void Deactivate() { active_ = false; }

private:
    KamataEngine::WorldTransform worldTransform_;
    std::unique_ptr<KamataEngine::Model> model_; ///< 経験値オーブ用モデル
    bool active_ = true;
    int32_t expValue_ = 0;

    KamataEngine::Vector3 velocity_{ 0, 0, 0 };
};