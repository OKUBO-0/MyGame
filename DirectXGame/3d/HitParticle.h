// HitParticle.h
#pragma once
#include <KamataEngine.h>

class HitParticle {
public:
    void Initialize(const KamataEngine::Vector3& pos);
    void Update();
    void Draw(KamataEngine::Camera* camera);
    bool IsActive() const { return active_; }

private:
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Model* model_ = nullptr;

    float lifetime_ = 0.3f;   ///< 火花の寿命（短め）
    float age_ = 0.0f;
    float alpha_ = 1.0f;
    bool active_ = true;

    KamataEngine::Vector3 velocity_; ///< 拡散方向の速度
};