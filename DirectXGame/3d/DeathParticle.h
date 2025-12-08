#pragma once
#include <KamataEngine.h>

class DeathParticle {
public:
    void Initialize(const KamataEngine::Vector3& pos);
    void Update();
    void Draw(KamataEngine::Camera* camera);
    bool IsActive() const { return active_; }

private:
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Model* model_ = nullptr;
    KamataEngine::ObjectColor* objectColor_ = nullptr;

    float lifetime_ = 0.6f;   // 煙の寿命
    float age_ = 0.0f;
    bool active_ = true;

    KamataEngine::Vector3 velocity_;
};