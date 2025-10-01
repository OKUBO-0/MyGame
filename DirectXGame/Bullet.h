#pragma once
#include <KamataEngine.h>

class Bullet {
public:
    Bullet();
    ~Bullet();

    // 弾の初期化
    void Initialize(const KamataEngine::Vector3& startPos, const KamataEngine::Vector3& direction, float speed = 0.5f);

    // 更新・描画
    void Update();
    void Draw();

    // 弾の状態
    bool IsActive() const { return active_; }

private:
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Model* model_ = nullptr;
    KamataEngine::Camera camera_;
    KamataEngine::Vector3 direction_;
    float speed_ = 0.5f;
    bool active_ = false;
};