#pragma once

#include "Player.h"
#include <KamataEngine.h>

class Enemy
{
public:
    // コンストラクタ
    Enemy();

    // デストラクタ
    ~Enemy();

    // 初期化
    void Initialize();

    // 更新
    void Update();

    // 描画
    void Draw();

    // 位置を設定する
    void SetPosition(const KamataEngine::Vector3& pos) {
        worldTransform_.translation_ = pos;
    }

    // プレイヤーを設定する
    void SetPlayer(Player* player) {
        player_ = player;
    }

private:
    // ワールドトランスフォーム
    KamataEngine::WorldTransform worldTransform_;

    // カメラ（描画用）
    KamataEngine::Camera camera_;

    // モデル
    KamataEngine::Model* enemyModel_ = nullptr;

    float speed_ = 0.1f;   // 移動速度
    float stopZ_ = 10.0f; // 停止するZ座標

    // プレイヤー参照
    Player* player_ = nullptr;
};