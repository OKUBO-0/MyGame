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
    player_->SetEnemyManager(&enemyManager_); // ★追加

    fade_.Initialize();
    fadeOutStarted_ = false;

    uint32_t blackTex = TextureManager::Load("uvChecker.png");
    pauseOverlay_ = Sprite::Create(blackTex, { 0,0 });
    pauseOverlay_->SetSize({ 100,100 });
    pauseOverlay_->SetColor({ 0,0,0,0.5f });
}

void GameScene::Update() {
    fade_.Update();

    if (input_->TriggerKey(DIK_ESCAPE) && fade_.GetState() == Fade::State::Stay) {
        paused_ = !paused_;
    }

    if (paused_) {
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

        return;
    }

    if (fadeOutStarted_ && fade_.IsFinished()) {
        finished_ = true;
    }

    player_->Update();
    enemyManager_.Update();

    for (auto bullet : player_->GetBullets()) {
        if (!bullet->IsActive()) continue;

        for (auto enemy : enemyManager_.GetEnemies()) {
            if (!enemy->IsActive()) continue;

            KamataEngine::Vector3 bPos = bullet->GetPosition();
            KamataEngine::Vector3 ePos = enemy->GetPosition();

            float dx = bPos.x - ePos.x;
            float dy = bPos.y - ePos.y;
            float dz = bPos.z - ePos.z;
            float distSq = dx * dx + dy * dy + dz * dz;

            if (distSq < 0.5f * 0.5f) {
                bullet->Deactivate();
                enemy->Deactivate();
            }
        }
    }
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