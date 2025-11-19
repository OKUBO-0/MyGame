#pragma once
#include "IScene.h"
#include "../2d/Fade.h"
#include "../2d/Score.h"
#include <KamataEngine.h>

class ResultScene : public IScene {
public:
    ResultScene();
    ~ResultScene();

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

    bool IsFinished() const override { return finished_; }

private:
    KamataEngine::DirectXCommon* dxCommon_ = nullptr;
    KamataEngine::Input* input_ = nullptr;
    KamataEngine::Audio* audio_ = nullptr;

    KamataEngine::Sprite* backgroundSprite_ = nullptr;
    KamataEngine::Sprite* resultSprite_ = nullptr;
	KamataEngine::Sprite* resultUI_ = nullptr;

    Fade fade_;
    bool fadeOutStarted_ = false;
    bool finished_ = false;

    // ✅ Score用
    Score* scoreUI_ = nullptr;
    int currentScore_ = 0;   // 現在のスコア
    int targetScore_ = 100;  // 目標スコア
};