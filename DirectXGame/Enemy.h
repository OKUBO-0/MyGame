#pragma once
#include "Player.h"
#include <KamataEngine.h>

class Player;
class Enemy {
public:
    Enemy();
    ~Enemy();

    void Initialize();
    void Update();
    void Draw();

    void SetPosition(const KamataEngine::Vector3& pos) { worldTransform_.translation_ = pos; }
    void SetPlayer(Player* player) { player_ = player; }

    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }
    void Deactivate() { active_ = false; }
    bool IsActive() const { return active_; }

    void TakeDamage(int damage); // ★追加

private:
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Camera camera_;
    KamataEngine::Model* enemyModel_ = nullptr;

    float speed_ = 0.05f;
    float stopZ_ = 10.0f;

    int hp_ = 3; // ★追加
    Player* player_ = nullptr;
    bool active_ = true;
};