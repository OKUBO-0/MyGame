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

    // プレイヤーを設定する
    void SetPlayer(Player* player) { player_ = player; }

private:
    // ワールドトランスフォーム
    KamataEngine::WorldTransform worldTransform_;

    // カメラ（描画用）
    KamataEngine::Camera camera_;

    // モデル
    KamataEngine::Model* enemyModel_ = nullptr;

    // プレイヤー参照
    Player* player_ = nullptr;
};