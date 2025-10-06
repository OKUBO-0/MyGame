#pragma once

#include "IScene.h"
#include "../Player.h"
#include "../EnemyManager.h"
#include "../SkyDome.h"
#include "../Fade.h"
#include <KamataEngine.h>

class GameScene : public IScene {
public:
    GameScene();
    ~GameScene();

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

    bool IsFinished() const override { return finished_; }

private:
    // エンジン関連
    KamataEngine::DirectXCommon* dxCommon_ = nullptr;
    KamataEngine::Input* input_ = nullptr;
    KamataEngine::Audio* audio_ = nullptr;
    KamataEngine::Camera camera_;

    // ゲームオブジェクト
    SkyDome* skyDome_ = nullptr;
    Player* player_ = nullptr;
    EnemyManager enemyManager_;

    // シーン制御
    Fade fade_;
    bool fadeOutStarted_ = false;
    bool finished_ = false;

    // ポーズ表示
    bool paused_ = false;
    KamataEngine::Sprite* pauseOverlay_ = nullptr; // 半透明背景
    KamataEngine::Sprite* pauseText_ = nullptr;    // "Paused"文字
};