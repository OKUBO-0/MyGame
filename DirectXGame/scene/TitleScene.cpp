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

    // --- タイトルロゴスプライト生成 ---
    uint32_t titleTex = TextureManager::Load("title/title.png");
    titleSprite_ = std::unique_ptr<Sprite>(Sprite::Create(titleTex, { 0, 0 }));
    titleSprite_->SetSize({ 1280, 720 });

    // --- タイトルUIスプライト生成 ---
    uint32_t titleUITex = TextureManager::Load("title/titleUI.png");
    titleUISprite_ = std::unique_ptr<Sprite>(Sprite::Create(titleUITex, { 0, 0 }));
    titleUISprite_->SetSize({ 1280, 720 });

    // --- カーソルスプライト生成 ---
    uint32_t cursorTex = TextureManager::Load("title/cursor.png");
    cursorSprite_ = std::unique_ptr<Sprite>(Sprite::Create(cursorTex, { 0, 0 }));
    cursorSprite_->SetSize({ 1280, 720 });

    // ガイド画面
    uint32_t guideUITex = TextureManager::Load("title/guideUI.png");
    guideSprite_ = std::unique_ptr<Sprite>(Sprite::Create(guideUITex, { 0, 0 }));
	guideSprite_->SetSize({1280, 720});

    // モデル生成（天球モデルを読み込み、背景の空を表現）
    playerModel_.reset(Model::CreateFromOBJ("octopus"));

    // 天球生成
	skyDome_ = std::make_unique<SkyDome>();
	skyDome_->Initialize();

    // ワールド変換初期化
    worldTransform_.Initialize();
    // モデルの向き（反対方向）
    worldTransform_.rotation_ = { 0.0f, -2.618f, 0.0f };
    // モデルの大きさ
    worldTransform_.scale_ = { 3.0f, 3.0f, 3.0f };
    // モデルの位置（右下に寄せる）
    worldTransform_.translation_ = { 20.0f, -10.0f, 0.0f };
    // 行列更新
    worldTransform_.UpdateMatrix();

    // --- フェードや演出用変数の初期化 ---
    fade_.Initialize();
    fadeOutStarted_ = false;

    // --- カメラ設定 --- 
    camera_.Initialize();
    camera_.rotation_ = { 0.0f, 0.0f, 0.0f }; 
    camera_.translation_ = { 0.0f, 0.0f, -50.0f }; 
    camera_.UpdateMatrix();
}

void TitleScene::Update() {
    // --- フェード更新（常に先頭で処理） ---
    fade_.Update();

    // --- BGM再生（ループ再生、未再生なら開始） ---
    if (!audio_->IsPlaying(titleBGMHandle_)) {
        titleBGMHandle_ = audio_->PlayWave(titleBGMHandle_, true, 0.1f);
    }

    // ガイド表示中は ESC で閉じる
    if (guideActive_) {
        if (input_->TriggerKey(DIK_ESCAPE)) {
            guideActive_ = false;
        }
        return; // メニュー操作は無効
    }

    // --- メニュー選択（W / S） ---
    if (input_->TriggerKey(DIK_W)) {
        menuIndex_ = std::max<int32_t>(0, menuIndex_ - 1);
    }
    if (input_->TriggerKey(DIK_S)) {
        menuIndex_ = std::min<int32_t>(2, menuIndex_ + 1);
    }

    // --- カーソル位置更新 ---
    switch (menuIndex_) {
    case 0: cursorSprite_->SetPosition({ 0, 0 }); break; // Play
    case 1: cursorSprite_->SetPosition({ 0, 120 }); break; // Guide
    case 2: cursorSprite_->SetPosition({ 0, 240 }); break; // Quit
    }

    // --- 決定（SPACE / ENTER） ---
    if (input_->TriggerKey(DIK_SPACE)) {
        switch (menuIndex_) {
        case 0: // Play
            if (fade_.GetState() == Fade::State::kStay) {
                fade_.StartFadeOut();
                fadeOutStarted_ = true;
                SetSceneNo(Scene::Game);
            }
            break;

        case 1: // Guide
            guideActive_ = true;
            break;

        case 2: // Quit
            PostQuitMessage(0); // Windowsアプリ終了
            break;
        }
    }

    // --- フェード完了 → シーン終了処理 ---
    if (fadeOutStarted_ && fade_.IsFinished()) {
        audio_->StopWave(titleBGMHandle_);
        finished_ = true;
    }

    worldTransform_.UpdateMatrix();
}

void TitleScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // --- 背景描画 ---
    Sprite::PreDraw(dxCommon->GetCommandList());
    Sprite::PostDraw();

    // --- モデル描画（タイトル演出用） ---
    dxCommon_->ClearDepthBuffer();
    Model::PreDraw();
    if (playerModel_) {
        playerModel_->Draw(worldTransform_, camera_);
    }
	skyDome_->Draw();
    Model::PostDraw();

    // --- UI・フェード描画 ---
    Sprite::PreDraw(dxCommon->GetCommandList());
    titleSprite_->Draw();
    titleUISprite_->Draw();
    // ガイド画面表示中
    if (guideActive_) {
        guideSprite_->Draw();
        return;
    }
    cursorSprite_->Draw();
    fade_.Draw();
    Sprite::PostDraw();
}

void TitleScene::Finalize() {
    // --- 特別な終了処理は不要（リソース解放はデストラクタで対応） ---
}