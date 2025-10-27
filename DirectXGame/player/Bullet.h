#pragma once

#include <KamataEngine.h>

class Bullet {
public:
    Bullet();
    ~Bullet();

    // 初期化と更新
    void Initialize(const KamataEngine::Vector3& startPos, const KamataEngine::Vector3& direction, float speed = 0.5f);
    void Update(const KamataEngine::Vector3& playerPos);
    void Draw(KamataEngine::Camera* camera);

    // 状態取得・操作
    bool IsActive() const { return active_; }
    void Deactivate() { active_ = false; }

    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }
    int GetPower() const { return power_; }

private:
    // エンジン関連
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Model* model_ = nullptr;

    // 弾の挙動
    KamataEngine::Vector3 direction_;
    float speed_ = 0.5f;
    int power_ = 1;

    // 状態管理
    bool active_ = false;
};