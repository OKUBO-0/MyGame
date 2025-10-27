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

    uint32_t readyTex = TextureManager::Load("ready.png");
	readyOverlay_ = Sprite::Create(readyTex, {0, 0});
	readyOverlay_->SetSize({1280, 720});

    uint32_t goTex = TextureManager::Load("go.png");
	goOverlay_ = Sprite::Create(goTex, {0, 0});
	goOverlay_->SetSize({1280, 720});

    startState_ = StartState::Ready;
    startTimer_ = 0;

    // SkyDome
    skyDome_ = new SkyDome();
    skyDome_->Initialize();

    // Player
    player_ = new Player();
    player_->Initialize();

    // EnemyManager
    // 初期Wave読み込み
    currentWave_ = 1;
    waveLoading_ = false;
    std::string csvPath = "Resources/csv/wave" + std::to_string(currentWave_) + ".csv";
    enemyManager_.Initialize(csvPath, player_);
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

    // ゲーム開始演出処理
    if (startState_ != StartState::Play) {
        startTimer_++;

        switch (startState_) {
        case StartState::Ready:
            if (startTimer_ > 60) {  // 1秒後にGOへ（60FPS前提）
                startState_ = StartState::Go;
                startTimer_ = 0;
            }
            break;

        case StartState::Go:
            if (startTimer_ > 60) {  // さらに1秒後にプレイ開始
                startState_ = StartState::Play;
            }
            break;
        }

        return; // Ready/Go中は他の処理を止める
    }

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

    // プレイヤーと敵の接触判定（HP減少）
    for (auto enemy : enemyManager_.GetEnemies()) {
        if (!enemy->IsActive()) continue;

        Vector3 ePos = enemy->GetPosition();
        Vector3 pPos = player_->GetWorldPosition();

        float dx = ePos.x - pPos.x;
        float dz = ePos.z - pPos.z;
        float distSq = dx * dx + dz * dz;

        if (distSq < 1.0f && !player_->IsInvincible()) {
            player_->TakeDamage();              // HPを1減らす
            // player_->StartInvincibility();  // 無敵時間を開始（必要なら）
            // audio_->Play("damage.wav");     // ダメージSE（任意）
        }
    }

    // Waveクリア判定
    bool allEnemiesDefeated = true;
    for (auto enemy : enemyManager_.GetEnemies()) {
        if (enemy->IsActive()) {
            allEnemiesDefeated = false;
            break;
        }
    }

    if (allEnemiesDefeated && !waveLoading_) {
        waveLoading_ = true;
        currentWave_++;

        std::string nextCSV = "Resources/csv/wave" + std::to_string(currentWave_) + ".csv";
        enemyManager_.Initialize(nextCSV, player_);
        player_->SetEnemyManager(&enemyManager_);
        waveLoading_ = false;
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
    enemyManager_.Draw(&player_->GetCamera());
    Model::PostDraw();

    // フェード描画
    Sprite::PreDraw(dxCommon->GetCommandList());
    if (startState_ == StartState::Ready) {
        readyOverlay_->Draw();
    }
    else if (startState_ == StartState::Go) {
        goOverlay_->Draw();
    }
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