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

    int GetPower() const { return power_; } // ★追加

private:
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Model* model_ = nullptr;
    KamataEngine::Camera camera_;
    KamataEngine::Vector3 direction_;
    float speed_ = 0.5f;
    int power_ = 1; // ★追加
    bool active_ = false;
};