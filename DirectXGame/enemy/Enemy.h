#pragma once

#include <KamataEngine.h>
#include "Player.h"
#include <memory>

class Player;

class Enemy {
public:
    void Initialize();
    void Update();
    void UpdateType2();
    void Draw(KamataEngine::Camera* camera);

    void SetPosition(const KamataEngine::Vector3& pos);
    void SetPlayer(Player* player);
    void SetModelByType(int32_t type);

    KamataEngine::Vector3 GetPosition() const;

    bool IsActive() const;
    void Deactivate();

    void SetHP(int32_t hp);
    int32_t GetHP() const;

    void TakeDamage(int32_t damage,
        const KamataEngine::Vector3& knockDir = { 0,0,0 },
        float strength = 0.0f);

    void SetEXP(int32_t exp);
    int32_t GetEXP() const;

    bool JustDied() const;
    void ResetJustDied();

    // 追加：EnemyManager から速度を設定する
    void SetSpeed(float speed) { speed_ = speed; }

private:
    KamataEngine::WorldTransform worldTransform_;
    std::unique_ptr<KamataEngine::Model> enemyModel_;

    float speed_;   // EnemyManager から上書きされる

    int32_t hp_;
    int32_t exp_;
    bool active_ = true;

    Player* player_ = nullptr;

    std::unique_ptr<KamataEngine::ObjectColor> objectColor_;
    float hitFlashTimer_ = 0.0f;
    static constexpr float kHitFlashDuration = 0.12f;
    uint32_t whiteTextureHandle_ = 0;

    KamataEngine::Vector3 knockbackVelocity_ = { 0,0,0 };
    float knockbackTimer_ = 0.0f;
    static constexpr float kKnockbackDuration = 0.18f;

    bool justDied_ = false;

    int enemyType_;
    float approachSpeed_ = 1.0f;
    float circleSpeed_ = 1.0f;

    KamataEngine::Audio* audio_ = nullptr;
    uint32_t deathSEHandle_ = 0;
};