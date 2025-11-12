#pragma once

#include <KamataEngine.h>
#include "Player.h"

class Player;

class Enemy {
public:
    Enemy();
    ~Enemy();

    void Initialize();                 
    void Update();
    void Draw(KamataEngine::Camera* camera);

    void SetPosition(const KamataEngine::Vector3& pos) { worldTransform_.translation_ = pos; }
    void SetPlayer(Player* player) { player_ = player; }
    void SetModelByType(int type);

    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }
    bool IsActive() const { return active_; }
    void Deactivate() { active_ = false; }

    void SetHP(int hp) { hp_ = hp; }
    int GetHP() const { return hp_; }
    void TakeDamage(int damage);

    void SetEXP(int exp) { exp_ = exp; }
    int GetEXP() const { return exp_; }

private:
    // エンジン関連
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Model* enemyModel_ = nullptr;

    // 敵の挙動
    float speed_ = 0.1f;

    // 状態管理
    int hp_ = 3;
    int exp_ = 0;
    bool active_ = true;

    // プレイヤー連携
    Player* player_ = nullptr;
};