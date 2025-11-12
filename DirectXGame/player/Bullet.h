#pragma once
#include <KamataEngine.h>

class Bullet {
public:
    Bullet();
    ~Bullet();

    void Initialize(const KamataEngine::Vector3& startPos, const KamataEngine::Vector3& direction, float speed = 0.5f);
    void Update(const KamataEngine::Vector3& playerPos);
    void Draw(KamataEngine::Camera* camera);

    bool IsActive() const { return active_; }
    void Deactivate() { active_ = false; }
    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

    // ✅ 攻撃力の設定と取得
    void SetDamage(int value) { damage_ = value; }
    int GetDamage() const { return damage_; }

private:
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Model* model_ = nullptr;

    KamataEngine::Vector3 direction_;
    float speed_ = 0.5f;

    bool active_ = false;

    // ✅ 攻撃力
    int damage_ = 1;
};