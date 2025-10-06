#pragma once
#include <KamataEngine.h>

class Bullet {
public:
    Bullet();
    ~Bullet();

    void Initialize(const KamataEngine::Vector3& startPos, const KamataEngine::Vector3& direction, float speed = 0.5f);
    void Update();
    void Draw();

    bool IsActive() const { return active_; }
    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }
    void Deactivate() { active_ = false; }

private:
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Model* model_ = nullptr;
    KamataEngine::Camera camera_;
    KamataEngine::Vector3 direction_;
    float speed_ = 0.5f;
    bool active_ = false;
};