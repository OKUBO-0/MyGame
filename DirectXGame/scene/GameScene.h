#pragma once

#include "IScene.h"
#include "../player/Player.h"
#include "../enemy/EnemyManager.h"
#include "../3d/SkyDome.h"
#include "../2d/Fade.h"
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

    enum class StartState {
        Ready,
        Go,
        Play
    };

    StartState startState_ = StartState::Ready;
    int startTimer_ = 0;

    KamataEngine::Sprite* readyOverlay_ = nullptr;
    KamataEngine::Sprite* goOverlay_ = nullptr;

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
    KamataEngine::Sprite* pauseOverlay_ = nullptr;
    KamataEngine::Sprite* pauseText_ = nullptr;

    // Wave制御
    int currentWave_ = 1;
    bool waveLoading_ = false;

    // 死亡演出
    KamataEngine::Sprite* deathOverlay_ = nullptr;
    float deathAlpha_ = 0.0f;
    bool deathFadeInStarted_ = false;
    bool deathFadeInComplete_ = false;
    bool gameStopped_ = false;

    // ✅ レベルアップ演出
    bool levelUpActive_ = false;
    KamataEngine::Sprite* levelUpOverlay_ = nullptr;
};