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

    // SkyDome
    skyDome_ = new SkyDome();
    skyDome_->Initialize();

    // Player
    player_ = new Player();
    player_->Initialize();

    // EnemyManager
    enemyManager_.Initialize("Resources/EnemyPos.csv", player_);
    player_->SetEnemyManager(&enemyManager_);

    // Fade
    fade_.Initialize();
    fadeOutStarted_ = false;

    // Pause UI
    uint32_t blackTex = TextureManager::Load("uvChecker.png");
    pauseOverlay_ = Sprite::Create(blackTex, { 0, 0 });
    pauseOverlay_->SetSize({ 100, 100 });
    pauseOverlay_->SetColor({ 0, 0, 0, 0.5f });
}

void GameScene::Update() {
	// フェード更新
    fade_.Update();

    // ポーズ切り替え
    if (input_->TriggerKey(DIK_ESCAPE) && fade_.GetState() == Fade::State::Stay) {
        paused_ = !paused_;
    }

    // ポーズ中の操作
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

    // ゲーム更新
    player_->Update();
    enemyManager_.Update();

    // 弾と敵の当たり判定
    for (auto bullet : player_->GetBullets()) {
        if (!bullet->IsActive()) continue;

        for (auto enemy : enemyManager_.GetEnemies()) {
            if (!enemy->IsActive()) continue;

            Vector3 bPos = bullet->GetPosition();
            Vector3 ePos = enemy->GetPosition();
            float dx = bPos.x - ePos.x;
            float dy = bPos.y - ePos.y;
            float dz = bPos.z - ePos.z;
            float distSq = dx * dx + dy * dy + dz * dz;

            if (distSq < 0.25f) {
                enemy->TakeDamage(bullet->GetPower());
                bullet->Deactivate();
            }
        }
    }

    // 敵が防衛ラインに到達したらプレイヤーにダメージ
    for (auto enemy : enemyManager_.GetEnemies()) {
        if (!enemy->IsActive()) continue;

        if (enemy->GetPosition().z <= 10.0f) {
            if (!player_->IsInvincible()) {
                player_->TakeDamage();
            }
        }
    }

    // プレイヤー死亡 → Resultシーンへ
    if (player_->IsDead() && fade_.GetState() == Fade::State::Stay) {
        fade_.StartFadeOut();
        fadeOutStarted_ = true;
        SetSceneNo(SCENE::Result);
    }

    // フェード完了後にシーン終了
    if (fadeOutStarted_ && fade_.IsFinished()) {
        finished_ = true;
    }
}

void GameScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // スプライト描画（前）
    Sprite::PreDraw(dxCommon->GetCommandList());
    Sprite::PostDraw();

    // モデル描画
    dxCommon_->ClearDepthBuffer();
    Model::PreDraw(dxCommon->GetCommandList());
    skyDome_->Draw();
    player_->Draw();
    enemyManager_.Draw();
    Model::PostDraw();

    // フェード描画
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