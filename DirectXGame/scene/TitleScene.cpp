#include "TitleScene.h"
using namespace KamataEngine;

TitleScene::TitleScene() {}
TitleScene::~TitleScene() {
    delete backgroundSprite_;
    delete titleSprite_;
    delete titleUISprite_;
}

void TitleScene::Initialize() {
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    uint32_t blackTex = TextureManager::Load("color/black.png");
    backgroundSprite_ = Sprite::Create(blackTex, { 0,0 });
    backgroundSprite_->SetSize({ 1280,720 });

    uint32_t titleTex = TextureManager::Load("title/title.png");
    titleSprite_ = Sprite::Create(titleTex, { 0,-300 });
    titleSprite_->SetSize({ 1280,720 });

    uint32_t titleUITex = TextureManager::Load("title/titleUI.png");
    titleUISprite_ = Sprite::Create(titleUITex, { 0,0 });
    titleUISprite_->SetSize({ 1280,720 });
    titleUISprite_->SetColor({ 1,1,1,0 });

    fade_.Initialize();
    fadeOutStarted_ = false;
}

void TitleScene::Update() {
    fade_.Update();

    if (input_->TriggerKey(DIK_SPACE) && fade_.GetState() == Fade::State::Stay) {
        fade_.StartFadeOut();
        fadeOutStarted_ = true;
        SetSceneNo(SCENE::Game);
    }

    if (fadeOutStarted_ && fade_.IsFinished()) {
        finished_ = true;
    }

    Vector2 pos = titleSprite_->GetPosition();
    if (pos.y < 0.0f) {
        pos.y += 3.0f;
        if (pos.y > 0.0f) pos.y = 0.0f;
        titleSprite_->SetPosition(pos);
    }
    else {
        static float timer = 0.0f;
        timer += 0.02f;
        float alpha = 0.5f + 0.5f * sinf(timer * 3.1415f * 2.0f);
        titleUISprite_->SetColor({ 1,1,1,alpha });
    }
}

void TitleScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    Sprite::PreDraw(dxCommon->GetCommandList());
    backgroundSprite_->Draw();
    Sprite::PostDraw();

    dxCommon_->ClearDepthBuffer();

    Sprite::PreDraw(dxCommon->GetCommandList());
    titleSprite_->Draw();
    titleUISprite_->Draw();
    fade_.Draw();
    Sprite::PostDraw();
}

void TitleScene::Finalize() {}