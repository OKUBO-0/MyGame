#include "GameScene.h"
using namespace KamataEngine;

GameScene::GameScene() {}
GameScene::~GameScene() {
    delete skyDome_;
    delete player_;
    delete pauseOverlay_;
    delete pauseText_;
    delete expGauge_;
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
    uint32_t pauseTex = TextureManager::Load("pause.png");
    pauseOverlay_ = Sprite::Create(pauseTex, { 0, 0 });
    pauseOverlay_->SetSize({ 1280, 720 });
    pauseOverlay_->SetColor({ 1, 1, 1, 1 });

    uint32_t deathTex = TextureManager::Load("death.png"); // 任意の画像
    deathOverlay_ = Sprite::Create(deathTex, { 0, 0 });
    deathOverlay_->SetSize({ 1280, 720 });
    deathOverlay_->SetColor({ 1, 1, 1, 0.0f }); // 最初は透明

    // レベルアップUI
    uint32_t levelUpTex = TextureManager::Load("levelUp.png");
    levelUpOverlay_ = Sprite::Create(levelUpTex, { 0, 0 });
    levelUpOverlay_->SetSize({ 1280, 720 });
    levelUpOverlay_->SetColor({ 1, 1, 1, 1 });

    // 経験値ゲージ生成
    expGauge_ = new ExpGauge();
    expGauge_->Initialize();
}

void GameScene::Update() {
    // フェード更新（常に先頭で処理）
    fade_.Update();

    // スタート演出（Ready → Go → Play）
    if (startState_ != StartState::Play) {
        startTimer_++;
        switch (startState_) {
        case StartState::Ready:
            if (startTimer_ > 60) {
                startState_ = StartState::Go;
                startTimer_ = 0;
            }
            break;
        case StartState::Go:
            if (startTimer_ > 60) {
                startState_ = StartState::Play;
            }
            break;
        }
        return;
    }

    // ポーズ切り替え（ESCキー）
    if (input_->TriggerKey(DIK_ESCAPE) && fade_.GetState() == Fade::State::Stay) {
        paused_ = !paused_;
    }

    // ポーズ中の選択処理
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

    // Waveクリア判定
    bool allEnemiesDefeated = true;
    for (auto enemy : enemyManager_.GetEnemies()) {
        if (enemy->IsActive()) {
            allEnemiesDefeated = false;
            break;
        }
    }

    if (allEnemiesDefeated && !waveLoading_) {
        const int MAX_WAVE = 10;

        if (currentWave_ >= MAX_WAVE) {
            // ✅ Wave10終了 → ResultSceneへ
            if (!fadeOutStarted_) {
                fade_.StartFadeOut();
                fadeOutStarted_ = true;
                SetSceneNo(SCENE::Result);
            }

            if (fadeOutStarted_ && fade_.IsFinished()) {
                finished_ = true;
            }

            return;
        }

        // ✅ 次Wave読み込み（Wave1〜9）
        waveLoading_ = true;
        currentWave_++;
        std::string nextCSV = "Resources/csv/wave" + std::to_string(currentWave_) + ".csv";
        enemyManager_.Initialize(nextCSV, player_);
        player_->SetEnemyManager(&enemyManager_);
        waveLoading_ = false;
    }

    // プレイヤー死亡 → ゲーム停止＋演出開始
    if (player_->IsDead() && !deathFadeInStarted_) {
        deathFadeInStarted_ = true;
        deathAlpha_ = 0.0f;
        gameStopped_ = true;
    }

    // 死亡演出中 → ゲーム停止
    if (gameStopped_) {
        if (deathFadeInStarted_ && !deathFadeInComplete_) {
            deathAlpha_ += 0.02f;
            if (deathAlpha_ >= 0.5f) {
                deathAlpha_ = 0.5f;
                deathFadeInComplete_ = true;
            }
            deathOverlay_->SetColor({ 1, 1, 1, deathAlpha_ });
        }

        if (deathFadeInComplete_ && input_->TriggerKey(DIK_SPACE)) {
            fade_.StartFadeOut();
            fadeOutStarted_ = true;
            SetSceneNo(SCENE::Result);
        }

        if (fadeOutStarted_ && fade_.IsFinished()) {
            finished_ = true;
        }

        return;
    }

    // 経験値ゲージ更新（EXPに連動）
    if (expGauge_) {
        expGauge_->SetEXP(player_->GetEXP(), player_->GetNextLevelEXP());
        expGauge_->SetLevel(player_->GetLevel());
        expGauge_->Update();
    }

    // レベルアップ要求検知 → 選択モードへ移行
    if (player_->IsLevelUpRequested()) {
        levelUpActive_ = true;
        player_->ClearLevelUpRequest();
        return;
    }

    // レベルアップ選択中 → 入力待ち
    if (levelUpActive_) {
        if (input_->TriggerKey(DIK_1)) {
            player_->UpgradeBulletPower();
            levelUpActive_ = false;
        }
        else if (input_->TriggerKey(DIK_2)) {
            player_->UpgradeBulletCooldown();
            levelUpActive_ = false;
        }
        else if (input_->TriggerKey(DIK_3)) {
            player_->RecoverHP();
            levelUpActive_ = false;
        }
        return;
    }

    // プレイヤー・敵の更新
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

            if (distSq < 1.0f) {
                enemy->TakeDamage(bullet->GetDamage());

                if (!enemy->IsActive()) {
                    player_->AddEXP(enemy->GetEXP());
                }

                bullet->Deactivate();
            }
        }
    }

    // プレイヤーと敵の接触判定（押し戻し＋ダメージ）
    for (auto enemy : enemyManager_.GetEnemies()) {
        if (!enemy->IsActive()) continue;

        Vector3 ePos = enemy->GetPosition();
        Vector3 pPos = player_->GetWorldPosition();
        float dx = ePos.x - pPos.x;
        float dz = ePos.z - pPos.z;
        float distSq = dx * dx + dz * dz;

        const float minDist = 3.0f;
        const float pushStrength = 1.0f;

        if (distSq < minDist * minDist && distSq > 0.0001f) {
            float dist = std::sqrt(distSq);
            float overlap = minDist - dist;
            float nx = dx / dist;
            float nz = dz / dist;
            ePos.x += nx * overlap * pushStrength;
            ePos.z += nz * overlap * pushStrength;
            enemy->SetPosition(ePos);

            if (!player_->IsInvincible()) {
                player_->TakeDamage();
            }
        }
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

    if (deathFadeInStarted_) {
        deathOverlay_->Draw();
    }

    if (startState_ == StartState::Ready) {
        readyOverlay_->Draw();
    }
    else if (startState_ == StartState::Go) {
        goOverlay_->Draw();
    }

    if (levelUpActive_) {
        levelUpOverlay_->Draw(); // ✅ レベルアップ画面
    }

    // ✅ 経験値ゲージ描画
    if (expGauge_) {
        expGauge_->Draw();
    }

    // ポーズ表示
    if (paused_) {
        Sprite::PreDraw(dxCommon->GetCommandList());
        pauseOverlay_->Draw();
        Sprite::PostDraw();
    }

    fade_.Draw();


    Sprite::PostDraw();
}

void GameScene::Finalize() {}