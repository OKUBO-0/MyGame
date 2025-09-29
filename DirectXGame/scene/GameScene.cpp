#include "GameScene.h"
using namespace KamataEngine;

GameScene::GameScene() {}
GameScene::~GameScene() {
    delete skyDome_;
    delete player_;
    delete pauseOverlay_;
    delete pauseText_;
}

void GameScene::Initialize() {
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    camera_.Initialize();

    skyDome_ = new SkyDome();
    skyDome_->Initialize();

    player_ = new Player();
    player_->Initialize();

    enemyManager_.Initialize("Resources/EnemyPos.csv", player_);

    fade_.Initialize();
    fadeOutStarted_ = false;

    // ポーズ画面用スプライト
    uint32_t blackTex = TextureManager::Load("uvChecker.png");
    pauseOverlay_ = Sprite::Create(blackTex, { 0,0 });
    pauseOverlay_->SetSize({ 100,100 });
    pauseOverlay_->SetColor({ 0,0,0,0.5f }); // 半透明
}

void GameScene::Update() {
    fade_.Update();

    // ESCでポーズ切り替え
    if (input_->TriggerKey(DIK_ESCAPE) && fade_.GetState() == Fade::State::Stay) {
        paused_ = !paused_;
    }

    if (paused_) {
        // ポーズ中の入力
        if (input_->TriggerKey(DIK_1)) {
            fade_.StartFadeOut();
            fadeOutStarted_ = true;
            SetSceneNo(SCENE::Title);
        }
        else if (input_->TriggerKey(DIK_2)) {
            fade_.StartFadeOut();
            fadeOutStarted_ = true;
            SetSceneNo(SCENE::Result);
        }

        if (fadeOutStarted_ && fade_.IsFinished()) {
            finished_ = true;
        }

        return; // ポーズ中はゲーム更新停止
    }

    if (fadeOutStarted_ && fade_.IsFinished()) {
        finished_ = true;
    }

    player_->Update();
    enemyManager_.Update();
}

void GameScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    Sprite::PreDraw(dxCommon->GetCommandList());
    Sprite::PostDraw();

    dxCommon_->ClearDepthBuffer();

    Model::PreDraw(dxCommon->GetCommandList());
    skyDome_->Draw();
    player_->Draw();
    enemyManager_.Draw();
    Model::PostDraw();

    // フェード
    Sprite::PreDraw(dxCommon->GetCommandList());
    fade_.Draw();
    Sprite::PostDraw();

    // ポーズ表示
    if (paused_) {
        Sprite::PreDraw(dxCommon->GetCommandList());
        pauseOverlay_->Draw();
        Sprite::PostDraw();
    }
}

void GameScene::Finalize() {}