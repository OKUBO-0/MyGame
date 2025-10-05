#pragma once
#include "IScene.h"
#include "../Fade.h"
#include "../TitleUI.h"
#include <KamataEngine.h>

class TitleScene : public IScene {
public:
    TitleScene();
    ~TitleScene();

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
    KamataEngine::Sprite* titleSprite_ = nullptr;
    KamataEngine::Sprite* titleUISprite_ = nullptr;

    TitleUI* titleUI_ = nullptr;

    Fade fade_;
    bool fadeOutStarted_ = false;
    bool finished_ = false;

    // タイトルBGM
    uint32_t titleBGMHandle_ = 0;

    // --- 演出用（Z座標で管理、逆方向） ---
    bool modelArrived_ = false;
    float modelTargetZ_ = 20.0f;
    float modelStartZ_ = 50.0f;
    float modelSpeed_ = -0.2f;

    // --- 回転フラグ ---
    bool startRotate_ = false;

    // --- 点滅制御用 ---
    float blinkTimer_ = 0.0f;
};