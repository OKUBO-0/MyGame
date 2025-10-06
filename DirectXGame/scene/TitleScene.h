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
    // エンジン関連
    KamataEngine::DirectXCommon* dxCommon_ = nullptr;
    KamataEngine::Input* input_ = nullptr;
    KamataEngine::Audio* audio_ = nullptr;

    // スプライトUI
    KamataEngine::Sprite* backgroundSprite_ = nullptr;
    KamataEngine::Sprite* titleSprite_ = nullptr;
    KamataEngine::Sprite* titleUISprite_ = nullptr;
    TitleUI* titleUI_ = nullptr;

    // フェード・シーン制御
    Fade fade_;
    bool fadeOutStarted_ = false;
    bool finished_ = false;

    // BGM
    uint32_t titleBGMHandle_ = 0;

    // モデル演出（Z軸移動）
    bool modelArrived_ = false;
    float modelTargetZ_ = 20.0f;
    float modelStartZ_ = 50.0f;
    float modelSpeed_ = -0.2f;

    // 回転演出
    bool startRotate_ = false;

    // 点滅演出
    float blinkTimer_ = 0.0f;
};