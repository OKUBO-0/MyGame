#include "ResultScene.h"
using namespace KamataEngine;

ResultScene::ResultScene() {}
ResultScene::~ResultScene() {
    delete backgroundSprite_;
    delete resultSprite_;
}

void ResultScene::Initialize() {
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    uint32_t blackTex = TextureManager::Load("color/black.png");
    backgroundSprite_ = Sprite::Create(blackTex, { 0,0 });
    backgroundSprite_->SetSize({ 1280,720 });

    uint32_t resultTex = TextureManager::Load("result/result.png");
    resultSprite_ = Sprite::Create(resultTex, { 0,0 });
    resultSprite_->SetSize({ 1280,720 });

    fade_.Initialize();
    fadeOutStarted_ = false;
}

void ResultScene::Update() {
    fade_.Update();

    if (input_->TriggerKey(DIK_RETURN) && fade_.GetState() == Fade::State::Stay) {
        fade_.StartFadeOut();
        fadeOutStarted_ = true;
        SetSceneNo(SCENE::Title); // タイトルへ戻る
    }

    if (fadeOutStarted_ && fade_.IsFinished()) {
        finished_ = true;
    }
}

void ResultScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    Sprite::PreDraw(dxCommon->GetCommandList());
    backgroundSprite_->Draw();
    resultSprite_->Draw();
    fade_.Draw();
    Sprite::PostDraw();
}

void ResultScene::Finalize() {}