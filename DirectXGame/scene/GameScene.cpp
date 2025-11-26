#include "GameScene.h"
using namespace KamataEngine;

GameScene::GameScene() {}
GameScene::~GameScene() {
    // 動的に生成したリソースを解放
    delete player_;
    delete pauseOverlay_;
    delete pauseText_;
    delete expGauge_;
    delete hpGauge_;
    delete waveUI_;
    delete gridPlane_;
    delete skyDome_;
}

void GameScene::Initialize() {
    // 各種シングルトンの取得（描画・入力・音声）
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    // カメラ初期化
    camera_.Initialize();

    // Ready/Go 演出用スプライト生成
    uint32_t readyTex = TextureManager::Load("ready.png");
    readyOverlay_ = Sprite::Create(readyTex, { 0, 0 });
    readyOverlay_->SetSize({ 1280, 720 });

    uint32_t goTex = TextureManager::Load("go.png");
    goOverlay_ = Sprite::Create(goTex, { 0, 0 });
    goOverlay_->SetSize({ 1280, 720 });

    // スタート演出の初期状態
    startState_ = StartState::Ready;
    startTimer_ = 0;

    // プレイヤー生成
    player_ = new Player();
    player_->Initialize();

    // 敵管理の初期化（Wave1を読み込み）
    currentWave_ = 1;
    waveLoading_ = false;
    std::string csvPath = "Resources/csv/wave" + std::to_string(currentWave_) + ".csv";
    enemyManager_.Initialize(csvPath, player_);
    player_->SetEnemyManager(&enemyManager_);

    // フェード初期化
    fade_.Initialize();
    fadeOutStarted_ = false;

    // ポーズ画面用スプライト生成
    uint32_t pauseTex = TextureManager::Load("pause.png");
    pauseOverlay_ = Sprite::Create(pauseTex, { 0, 0 });
    pauseOverlay_->SetSize({ 1280, 720 });
    pauseOverlay_->SetColor({ 1, 1, 1, 1 });

    // 死亡演出用スプライト生成（最初は透明）
    uint32_t deathTex = TextureManager::Load("death.png");
    deathOverlay_ = Sprite::Create(deathTex, { 0, 0 });
    deathOverlay_->SetSize({ 1280, 720 });
    deathOverlay_->SetColor({ 1, 1, 1, 0.0f });

    // レベルアップ演出用スプライト生成
    uint32_t levelUpTex = TextureManager::Load("levelUp.png");
    levelUpOverlay_ = Sprite::Create(levelUpTex, { 0, 0 });
    levelUpOverlay_->SetSize({ 1280, 720 });
    levelUpOverlay_->SetColor({ 1, 1, 1, 1 });

    // 各種UI生成
    expGauge_ = new ExpGauge();
    expGauge_->Initialize();

    hpGauge_ = new HpGauge();
    hpGauge_->Initialize();

    waveUI_ = new WaveUI();
    waveUI_->Initialize();
    waveUI_->SetWave(currentWave_);

    // 背景要素生成
    gridPlane_ = new GridPlane();
    gridPlane_->Initialize();

    skyDome_ = new SkyDome();
    skyDome_->Initialize();
}

void GameScene::Update() {
    // フェード更新（最優先で処理）
    fade_.Update();

    // Ready→Go→Play の開始演出
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
        return; // Play開始まではゲーム進行を止める
    }

    // ESCキーでポーズ切り替え（フェード中は無効）
    if (input_->TriggerKey(DIK_ESCAPE) && fade_.GetState() == Fade::State::Stay) {
        paused_ = !paused_;
    }

    // ポーズ中の処理（シーン遷移選択）
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
            GameData::totalEXP = player_->GetTotalEXP();
            finished_ = true;
        }
        return;
    }

    // Waveクリア判定（全敵撃破）
    bool allEnemiesDefeated = true;
    for (auto enemy : enemyManager_.GetEnemies()) {
        if (enemy->IsActive()) {
            allEnemiesDefeated = false;
            break;
        }
    }

    if (allEnemiesDefeated && !waveLoading_) {
        const int MAX_WAVE = 1;

        if (currentWave_ >= MAX_WAVE) {
            // 最終Wave終了 → ResultSceneへ遷移
            if (!fadeOutStarted_) {
                fade_.StartFadeOut();
                fadeOutStarted_ = true;
                SetSceneNo(SCENE::Result);
            }

            if (fadeOutStarted_ && fade_.IsFinished()) {
                GameData::totalEXP = player_->GetTotalEXP();
                finished_ = true;
            }

            return;
        }

        // 次Wave読み込み
        waveLoading_ = true;
        currentWave_++;
        std::string nextCSV = "Resources/csv/wave" + std::to_string(currentWave_) + ".csv";
        enemyManager_.Initialize(nextCSV, player_);
        player_->SetEnemyManager(&enemyManager_);
        waveLoading_ = false;
    }

    // HPゲージ更新
    if (hpGauge_) {
        hpGauge_->SetHP(player_->GetHP(), player_->GetMaxHP());
        hpGauge_->Update();
    }

    // WaveUI更新
    if (waveUI_) {
        waveUI_->SetWave(currentWave_);
        waveUI_->Update();
    }

    // 背景要素更新
    if (gridPlane_) {
        gridPlane_->Update();
    }
    if (skyDome_) {
        skyDome_->Update();
    }

    // プレイヤー死亡時の演出開始判定
    if (player_->IsDead() && hpGauge_->IsDepleted() && !deathFadeInStarted_) {
        deathFadeInStarted_ = true;
        deathAlpha_ = 0.0f;
        gameStopped_ = true;
    }

    // 死亡演出中はゲーム進行停止
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
            GameData::totalEXP = player_->GetTotalEXP();
            finished_ = true;
        }

        return;
    }

    // 経験値ゲージ更新
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

    // レベルアップ選択中の処理（強化項目を選択）
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

    // プレイヤーと敵の更新処理
    player_->Update();
    enemyManager_.Update();

    // 弾と敵の当たり判定処理
    for (auto bullet : player_->GetBullets()) {
        if (!bullet->IsActive()) continue;

        for (auto enemy : enemyManager_.GetEnemies()) {
            if (!enemy->IsActive()) continue;

            // 弾と敵の距離を計算
            Vector3 bPos = bullet->GetPosition();
            Vector3 ePos = enemy->GetPosition();
            float dx = bPos.x - ePos.x;
            float dy = bPos.y - ePos.y;
            float dz = bPos.z - ePos.z;
            float distSq = dx * dx + dy * dy + dz * dz;

            // 一定距離以内ならヒット判定
            if (distSq < 1.0f) {
                enemy->TakeDamage(bullet->GetDamage());

                // 敵が倒れた場合は経験値を加算
                if (!enemy->IsActive()) {
                    player_->AddEXP(enemy->GetEXP());
                }

                // 弾を消去
                bullet->Deactivate();
            }
        }
    }

    // プレイヤーと敵の接触判定（近すぎる場合は押し戻し＋ダメージ）
    for (auto enemy : enemyManager_.GetEnemies()) {
        if (!enemy->IsActive()) continue;

        Vector3 ePos = enemy->GetPosition();
        Vector3 pPos = player_->GetWorldPosition();
        float dx = ePos.x - pPos.x;
        float dz = ePos.z - pPos.z;
        float distSq = dx * dx + dz * dz;

        const float minDist = 3.0f;       // 最低限保つべき距離
        const float pushStrength = 1.0f;  // 押し戻しの強さ

        if (distSq < minDist * minDist && distSq > 0.0001f) {
            float dist = std::sqrt(distSq);
            float overlap = minDist - dist;

            // 正規化ベクトル（押し戻す方向）
            float nx = dx / dist;
            float nz = dz / dist;

            // 敵を押し戻す
            ePos.x += nx * overlap * pushStrength;
            ePos.z += nz * overlap * pushStrength;
            enemy->SetPosition(ePos);

            // プレイヤーが無敵でなければダメージを受ける
            if (!player_->IsInvincible()) {
                player_->TakeDamage();
            }
        }
    }
}

void GameScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // スプライト描画の前処理（背景やUIを描画する準備）
    Sprite::PreDraw(dxCommon->GetCommandList());
    Sprite::PostDraw();

    // モデル描画の前処理（3Dオブジェクトを描画する準備）
    dxCommon_->ClearDepthBuffer();
    Model::PreDraw();

    // 背景要素の描画（床グリッド）
    if (gridPlane_) {
        gridPlane_->Draw(&player_->GetCamera());
    }

    // 背景要素の描画（スカイドーム）
    if (skyDome_) {
        skyDome_->Draw();
    }

    // プレイヤー描画
    if (player_) {
        player_->Draw();
    }

    // 敵描画（Ready/Go中は非表示、Play開始後のみ表示）
    if (startState_ == StartState::Play) {
        enemyManager_.Draw(&player_->GetCamera());
    }

    // モデル描画の後処理
    Model::PostDraw();

    // スプライト描画の前処理（UIや演出を描画する準備）
    Sprite::PreDraw(dxCommon->GetCommandList());

    // 死亡演出中のオーバーレイ描画
    if (deathFadeInStarted_) {
        deathOverlay_->Draw();
    }

    // スタート演出（Ready/Go）のオーバーレイ描画
    if (startState_ == StartState::Ready) {
        readyOverlay_->Draw();
    }
    else if (startState_ == StartState::Go) {
        goOverlay_->Draw();
    }

    // レベルアップ選択画面の描画
    if (levelUpActive_) {
        levelUpOverlay_->Draw();
    }

    // 経験値ゲージ描画
    if (expGauge_) {
        expGauge_->Draw();
    }

    // HPゲージ描画（プレイヤーが生存中かつレベルアップ・ポーズ中でない場合のみ）
    if (hpGauge_ && player_ && !player_->IsDead() && !levelUpActive_ && !paused_) {
        hpGauge_->Draw();
    }

    // WaveUI描画（現在のWaveを表示）
    if (waveUI_) {
        waveUI_->Draw();
    }

    // ポーズ中のオーバーレイ描画
    if (paused_) {
        Sprite::PreDraw(dxCommon->GetCommandList());
        pauseOverlay_->Draw();
        Sprite::PostDraw();
    }

    // フェード演出描画（シーン遷移時の暗転）
    fade_.Draw();

    // スプライト描画の後処理
    Sprite::PostDraw();
}

void GameScene::Finalize() {}