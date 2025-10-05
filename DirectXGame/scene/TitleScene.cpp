#include "TitleScene.h"
#include <cmath>
using namespace KamataEngine;

TitleScene::TitleScene() {}
TitleScene::~TitleScene() {
    delete backgroundSprite_;
    delete titleSprite_;
    delete titleUISprite_;
    delete titleUI_;
}

void TitleScene::Initialize() {
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    // ----- オーディオ -----
    titleBGMHandle_ = audio_->LoadWave("Sounds/title.wav"); // BGMファイル名

    uint32_t blackTex = TextureManager::Load("color/black.png");
    backgroundSprite_ = Sprite::Create(blackTex, { 0,0 });
    backgroundSprite_->SetSize({ 1280,720 });

    uint32_t titleTex = TextureManager::Load("title/title.png");
    titleSprite_ = Sprite::Create(titleTex, { 0,0 });
    titleSprite_->SetSize({ 1280,720 });
    titleSprite_->SetColor({ 1,1,1,0 }); // 最初は非表示

    uint32_t titleUITex = TextureManager::Load("title/titleUI.png");
    titleUISprite_ = Sprite::Create(titleUITex, { 0,0 });
    titleUISprite_->SetSize({ 1280,720 });
    titleUISprite_->SetColor({ 1,1,1,0 }); // 最初は非表示

    titleUI_ = new TitleUI();
    titleUI_->Initialize();

    // モデルの初期位置を手前に
    titleUI_->SetPositionZ(modelStartZ_);

    fade_.Initialize();
    fadeOutStarted_ = false;
    modelArrived_ = false;
    startRotate_ = false;
    blinkTimer_ = 0.0f;
}

void TitleScene::Update() {
    fade_.Update();

    // タイトルBGM再生
    if (!audio_->IsPlaying(titleBGMHandle_)) {
        titleBGMHandle_ = audio_->PlayWave(titleBGMHandle_, true, 0.5f);
    }

    // モデルの移動処理（手前→奥）
    if (!modelArrived_) {
        float z = titleUI_->GetPositionZ();
        z += modelSpeed_;
        if (z <= modelTargetZ_) {
            z = modelTargetZ_;
            modelArrived_ = true;

            // モデル到着 → タイトル表示開始
            titleSprite_->SetColor({ 1,1,1,1 });
            titleUISprite_->SetColor({ 1,1,1,1 });

            // 回転開始フラグON
            startRotate_ = true;
        }
        titleUI_->SetPositionZ(z);
    }

    // モデル回転処理
    if (startRotate_) {
        titleUI_->AddRotationY(0.02f); // Y軸回転を加算
    }

    // titleUISprite_ 点滅処理
    if (modelArrived_) {
        blinkTimer_ += 0.05f;
        float alpha = (std::sin(blinkTimer_) * 0.5f) + 0.5f; // 0～1の範囲
        titleUISprite_->SetColor({ 1,1,1,alpha });
    }

    // 入力処理（モデル到達後のみ有効）
    if (modelArrived_) {
        if (input_->TriggerKey(DIK_RETURN) && fade_.GetState() == Fade::State::Stay) {
            fade_.StartFadeOut();
            fadeOutStarted_ = true;
            SetSceneNo(SCENE::Game);
        }
    }

    if (fadeOutStarted_ && fade_.IsFinished()) {
        audio_->StopWave(titleBGMHandle_);
        finished_ = true;
    }

    titleUI_->Update();
}

void TitleScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    Sprite::PreDraw(dxCommon->GetCommandList());
    backgroundSprite_->Draw();
    Sprite::PostDraw();

    dxCommon_->ClearDepthBuffer();

    Model::PreDraw(dxCommon->GetCommandList());
    titleUI_->Draw();   // モデル描画
    Model::PostDraw();

    Sprite::PreDraw(dxCommon->GetCommandList());
    titleSprite_->Draw();
    titleUISprite_->Draw();   // 点滅処理が反映される
    fade_.Draw();
    Sprite::PostDraw();
}

void TitleScene::Finalize() {}