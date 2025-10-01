#pragma once

#include <KamataEngine.h>
#include <vector>
#include "Bullet.h"

class Player
{
public:
    Player();
    ~Player();

    void Initialize();
    void Update();
    void Draw();

    KamataEngine::Vector3 GetWorldPosition() const {
        return worldTransform_.translation_;
    }

private:
    // 入力インスタンス
    KamataEngine::Input* input_ = nullptr;

    // ワールドトランスフォーム
    KamataEngine::WorldTransform worldTransform_;

    // カメラ
    KamataEngine::Camera camera_;

    // プレイヤーモデル
    KamataEngine::Model* playerModel_ = nullptr;

    float speed_ = 0.3f;

    // 弾管理
    std::vector<Bullet*> bullets_;
    float bulletCooldown_ = 1.0f; // 1秒間隔
    float bulletTimer_ = 0.0f;
};