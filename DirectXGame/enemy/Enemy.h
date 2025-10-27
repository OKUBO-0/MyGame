#pragma once

#include <KamataEngine.h>
#include "Player.h"

class Player;

class Enemy {
public:
    Enemy();
    ~Enemy();

    // 基本操作
    void Initialize();                 // モデル以外の初期化
    void Update();
    void Draw(KamataEngine::Camera* camera);

    // 外部連携
    void SetPosition(const KamataEngine::Vector3& pos) { worldTransform_.translation_ = pos; }
    void SetPlayer(Player* player) { player_ = player; }
    void SetModelByType(int type);    // モデル切り替え

    // 状態取得・操作
    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }
    bool IsActive() const { return active_; }
    void Deactivate() { active_ = false; }

    void TakeDamage(int damage);

private:
    // エンジン関連
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Model* enemyModel_ = nullptr;

    // 敵の挙動
    float speed_ = 0.1f;

    // 状態管理
    int hp_ = 3;
    bool active_ = true;

    // プレイヤー連携
    Player* player_ = nullptr;
};