#include "TitleScene.h"
#include <cmath>
using namespace KamataEngine;

TitleScene::TitleScene() {}
TitleScene::~TitleScene() {
    delete backgroundSprite_;
    delete titleSprite_;
    delete titleUISprite_;
    delete titleUI_;
	delete ruleSprite_;
}

void TitleScene::Initialize() {
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    // BGM読み込み
    titleBGMHandle_ = audio_->LoadWave("Sounds/title.wav");
	selectSEHandle_ = audio_->LoadWave("Sounds/select.wav");

    // 背景スプライト
    uint32_t blackTex = TextureManager::Load("color/black.png");
    backgroundSprite_ = Sprite::Create(blackTex, { 0, 0 });
    backgroundSprite_->SetSize({ 1280, 720 });

    // タイトルロゴ
    uint32_t titleTex = TextureManager::Load("title/title.png");
    titleSprite_ = Sprite::Create(titleTex, { 0, 0 });
    titleSprite_->SetSize({ 1280, 720 });
    titleSprite_->SetColor({ 1, 1, 1, 0 }); // 非表示

    // タイトルUI（点滅用）
    uint32_t titleUITex = TextureManager::Load("title/titleUI.png");
    titleUISprite_ = Sprite::Create(titleUITex, { 0, 0 });
    titleUISprite_->SetSize({ 1280, 720 });
    titleUISprite_->SetColor({ 1, 1, 1, 0 }); // 非表示

    // ルール説明UI
	uint32_t ruleTex = TextureManager::Load("title/rule.png");
	ruleSprite_ = Sprite::Create(ruleTex, {0, 0});
	ruleSprite_->SetSize({1280, 720});

    // モデル演出
    titleUI_ = new TitleUI();
    titleUI_->Initialize();
    titleUI_->SetPositionZ(modelStartZ_);

    // フェード初期化
    fade_.Initialize();
    fadeOutStarted_ = false;
    modelArrived_ = false;
    startRotate_ = false;
    blinkTimer_ = 0.0f;
}

void TitleScene::Update() {
    fade_.Update();

    // BGM再生（ループ）
    if (!audio_->IsPlaying(titleBGMHandle_)) {
        titleBGMHandle_ = audio_->PlayWave(titleBGMHandle_, true, 0.5f);
    }

    // モデル移動（手前→奥）
    if (!modelArrived_) {
        float z = titleUI_->GetPositionZ();
        z += modelSpeed_;
        if (z <= modelTargetZ_) {
            z = modelTargetZ_;
            modelArrived_ = true;

            // 到達 → タイトル表示・回転開始
            titleSprite_->SetColor({ 1, 1, 1, 1 });
            titleUISprite_->SetColor({ 1, 1, 1, 1 });
            startRotate_ = true;
        }
        titleUI_->SetPositionZ(z);
    }

    // モデル回転
    if (startRotate_) {
        titleUI_->AddRotation(0.02f);
    }

    // UI点滅
    if (modelArrived_) {
        blinkTimer_ += 0.05f;
        float alpha = std::sin(blinkTimer_) * 0.5f + 0.5f;
        titleUISprite_->SetColor({ 1, 1, 1, alpha });
    }

    // ルール表示切替
    if (input_->TriggerKey(DIK_RETURN) && !showRule_) {
        selectSEHandle_ = audio_->PlayWave(selectSEHandle_, false, 1.0f);
        showRule_ = true;
    }

    // 入力処理（Enterでゲーム開始）
    if (modelArrived_ && input_->TriggerKey(DIK_SPACE) && showRule_ && fade_.GetState() == Fade::State::Stay) {
        selectSEHandle_ = audio_->PlayWave(selectSEHandle_, false, 1.0f);
        showRule_ = false;
        fade_.StartFadeOut();
        fadeOutStarted_ = true;
        SetSceneNo(SCENE::Game);
    }

    // フェード完了 → シーン終了
    if (fadeOutStarted_ && fade_.IsFinished()) {
        audio_->StopWave(titleBGMHandle_);
        finished_ = true;
    }

    titleUI_->Update();
}

void TitleScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // 背景
    Sprite::PreDraw(dxCommon->GetCommandList());
    backgroundSprite_->Draw();
    Sprite::PostDraw();

    // モデル
    dxCommon_->ClearDepthBuffer();
    Model::PreDraw(dxCommon->GetCommandList());
    titleUI_->Draw();
    Model::PostDraw();

    // UI・フェード
    Sprite::PreDraw(dxCommon->GetCommandList());
    titleSprite_->Draw();
    titleUISprite_->Draw();
    if (showRule_) {
        ruleSprite_->Draw();
    }
    fade_.Draw();
    Sprite::PostDraw();
}

void TitleScene::Finalize() {}