#pragma once
#include <KamataEngine.h>

class Bullet {
public:
    virtual ~Bullet() = default;

    virtual void Initialize(const KamataEngine::Vector3& startPos, int power) {
        worldTransform_.Initialize();
        worldTransform_.translation_ = startPos;
        power_ = power;
        active_ = true;
    }

    virtual void Update(const KamataEngine::Vector3& playerPos) = 0; // ★純粋仮想

    virtual void Draw(KamataEngine::Camera* camera) {
        if (active_ && model_) {
            model_->Draw(worldTransform_, *camera);
        }
    }

    bool IsActive() const { return active_; }

protected:
    KamataEngine::WorldTransform worldTransform_;
    std::unique_ptr<KamataEngine::Model> model_;
    bool active_ = false;
    int power_ = 1;
};