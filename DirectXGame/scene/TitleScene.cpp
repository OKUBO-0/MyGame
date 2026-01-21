#include "TitleScene.h"
#include <cmath>

using namespace KamataEngine;

void TitleScene::Initialize() {
    // --- 各種シングルトン取得（描画・入力・音声） ---
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    // --- タイトル画面用BGMと効果音を読み込み ---
    titleBGMHandle_ = audio_->LoadWave("Sounds/bgm_title.wav");
    selectSEHandle_ = audio_->LoadWave("Sounds/se_select.wav");

    // --- 背景スプライト生成（黒背景） ---
    uint32_t blackTex = TextureManager::Load("color/black.png");
    backgroundSprite_ = std::unique_ptr<Sprite>(Sprite::Create(blackTex, { 0, 0 }));
    backgroundSprite_->SetSize({ 1280, 720 });

    // --- タイトルロゴスプライト生成（初期状態は非表示） ---
    uint32_t titleTex = TextureManager::Load("title/title.png");
    titleSprite_ = std::unique_ptr<Sprite>(Sprite::Create(titleTex, { 0, 0 }));
    titleSprite_->SetSize({ 1280, 720 });
    titleSprite_->SetColor({ 1, 1, 1, 0 });

    // --- タイトルUIスプライト生成（点滅演出用、初期状態は非表示） ---
    uint32_t titleUITex = TextureManager::Load("title/titleUI.png");
    titleUISprite_ = std::unique_ptr<Sprite>(Sprite::Create(titleUITex, { 0, 0 }));
    titleUISprite_->SetSize({ 1280, 720 });
    titleUISprite_->SetColor({ 1, 1, 1, 0 });

    // --- モデル演出用UI生成（奥から手前へ移動して登場） ---
    titleUI_ = std::make_unique<TitleUI>();
    titleUI_->Initialize();
    titleUI_->SetPositionZ(kModelStartZ);

    // --- フェードや演出用変数の初期化 ---
    fade_.Initialize();
    fadeOutStarted_ = false;
    modelArrived_ = false;
    startRotate_ = false;
    blinkTimer_ = 0.0f;
}

void TitleScene::Update() {
    // --- フェード更新（常に先頭で処理） ---
    fade_.Update();

    // --- BGM再生（ループ再生、未再生なら開始） ---
    if (!audio_->IsPlaying(titleBGMHandle_)) {
        titleBGMHandle_ = audio_->PlayWave(titleBGMHandle_, true, 0.1f);
    }

    // --- モデル移動（奥から手前へ移動） ---
    if (!modelArrived_) {
        float z = titleUI_->GetPositionZ();
        z += kModelSpeed;
        if (z <= modelTargetZ_) {
            z = modelTargetZ_;
            modelArrived_ = true;

            // モデル到達 → タイトルロゴ表示＆回転開始
            titleSprite_->SetColor({ 1, 1, 1, 1 });
            titleUISprite_->SetColor({ 1, 1, 1, 1 });
            startRotate_ = true;
        }
        titleUI_->SetPositionZ(z);
    }

    // --- モデル回転演出 ---
    if (startRotate_) {
        static constexpr float kRotateSpeed = 0.02f;
        titleUI_->AddRotation(kRotateSpeed);
    }

    // --- タイトルUI点滅演出（sin波でアルファ値を変化） ---
    if (modelArrived_) {
        static constexpr float kBlinkIncrement = 0.05f;
        blinkTimer_ += kBlinkIncrement;
        float alpha = std::sin(blinkTimer_) * 0.5f + 0.5f;
        titleUISprite_->SetColor({ 1, 1, 1, alpha });
    }

    // --- 入力処理（Enterキーでゲーム開始） ---
    if (modelArrived_ && input_->TriggerKey(DIK_SPACE) && fade_.GetState() == Fade::State::kStay) {
        selectSEHandle_ = audio_->PlayWave(selectSEHandle_, false, 1.0f);
        fade_.StartFadeOut();
        fadeOutStarted_ = true;
        SetSceneNo(Scene::Game);
    }

    // --- フェード完了 → シーン終了処理 ---
    if (fadeOutStarted_ && fade_.IsFinished()) {
        audio_->StopWave(titleBGMHandle_);
        finished_ = true;
    }

    // --- モデル更新処理 ---
    titleUI_->Update();
}

void TitleScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // --- 背景描画 ---
    Sprite::PreDraw(dxCommon->GetCommandList());
    backgroundSprite_->Draw();
    Sprite::PostDraw();

    // --- モデル描画（タイトル演出用） ---
    dxCommon_->ClearDepthBuffer();
    Model::PreDraw();
    titleUI_->Draw();
    Model::PostDraw();

    // --- UI・フェード描画 ---
    Sprite::PreDraw(dxCommon->GetCommandList());
    titleSprite_->Draw();
    titleUISprite_->Draw();
    fade_.Draw();
    Sprite::PostDraw();
}

void TitleScene::Finalize() {
    // --- 特別な終了処理は不要（リソース解放はデストラクタで対応） ---
}