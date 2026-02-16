#include "GameScene.h"
#include <random> // 追加: std::mt19937, std::random_device, std::shuffle 用
using namespace KamataEngine;

void GameScene::Initialize() {
    // 各種シングルトンの取得（描画・入力・音声）
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    pauseSEHandle_ = Audio::GetInstance()->LoadWave("Sounds/se_pause.wav");

    // カメラ初期化（メインカメラを準備）
    camera_.Initialize();

    // Ready/Go 演出用スプライト生成
    uint32_t readyTex = TextureManager::Load("ready.png");
    readyOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(readyTex, { 0, 0 }));
    readyOverlay_->SetSize({ 1280, 720 });

    uint32_t goTex = TextureManager::Load("go.png");
    goOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(goTex, { 0, 0 }));
    goOverlay_->SetSize({ 1280, 720 });

    uint32_t guideTex = TextureManager::Load("guide.png");
	guide_ = std::unique_ptr<Sprite>(Sprite::Create(guideTex, {0, 0}));
	guide_->SetSize({1280, 720});

    // スタート演出の初期状態
    startState_ = StartState::Ready;
    startTimer_ = 0;

    // プレイヤー生成
    player_ = std::make_unique<Player>();
    player_->Initialize();

    // 敵管理の初期化（Wave1を読み込み）
    currentWave_ = 1;
    waveLoading_ = false;
    std::string csvPath = "Resources/csv/wave" + std::to_string(currentWave_) + ".csv";
    enemyManager_.Initialize(csvPath, player_.get());
    player_->SetEnemyManager(&enemyManager_);

    // フェード初期化
    fade_.Initialize();
    fadeOutStarted_ = false;

    // ポーズ画面用スプライト生成
    uint32_t pauseTex = TextureManager::Load("pause.png");
    pauseOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(pauseTex, { 0, 0 }));
    pauseOverlay_->SetSize({ 1280, 720 });
    pauseOverlay_->SetColor({ 1, 1, 1, 1 });

    // 死亡演出用スプライト生成（最初は透明）
    uint32_t deathTex = TextureManager::Load("death.png");
    deathOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(deathTex, { 0, 0 }));
    deathOverlay_->SetSize({ 1280, 720 });
    deathOverlay_->SetColor({ 1, 1, 1, 0.0f });

    // レベルアップ演出用スプライト生成
    uint32_t levelUpTex = TextureManager::Load("levelUp.png");
    levelUpOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(levelUpTex, { 0, 0 }));

    // 矢印スプライト生成
    uint32_t arrowTex = TextureManager::Load("arrow.png");
    arrowSprite_ = std::unique_ptr<Sprite>(Sprite::Create(arrowTex, { 0, 0 }));

    uint32_t keyTexW = TextureManager::Load("key_w.png");
    uint32_t keyTexA = TextureManager::Load("key_a.png");
    uint32_t keyTexS = TextureManager::Load("key_s.png");
    uint32_t keyTexD = TextureManager::Load("key_d.png");

    keyW_ = std::unique_ptr<Sprite>(Sprite::Create(keyTexW, { 0, 0 }));
    keyA_ = std::unique_ptr<Sprite>(Sprite::Create(keyTexA, { 0, 0 }));
    keyS_ = std::unique_ptr<Sprite>(Sprite::Create(keyTexS, { 0, 0 }));
    keyD_ = std::unique_ptr<Sprite>(Sprite::Create(keyTexD, { 0, 0 }));
	ESC_ui_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ESC_ui.png"), {0, 0}));

    keyW_->SetSize({1280, 720});
	keyA_->SetSize({1280, 720});
	keyS_->SetSize({1280, 720});
	keyD_->SetSize({1280, 720});
	ESC_ui_->SetSize({1280, 720});

    // 各種UI生成
    expGauge_ = std::make_unique<ExpGauge>();
    expGauge_->Initialize();

    hpGauge_ = std::make_unique<HpGauge>();
    hpGauge_->Initialize();

    waveUI_ = std::make_unique<WaveUI>();
    waveUI_->Initialize();
    waveUI_->SetWave(currentWave_);

    // 背景要素生成
    gridPlane_ = std::make_unique<GridPlane>();
    gridPlane_->Initialize();

    skyDome_ = std::make_unique<SkyDome>();
    skyDome_->Initialize();

    // ★ ゲーム開始時にスコア系をリセット 
    GameData::totalExp = 0; 
    GameData::finalLevel = 1;
    GameData::totalKillCount = 0;

    // ★ レベルアップ選択肢の登録（画像を読み込む）
    levelUpOptions_.push_back({
        "通常弾強化",
        [](Player* p) { p->UpgradeNormalBullets(); },
        TextureManager::Load("lvup_normal.png")
        });

    levelUpOptions_.push_back({
        "周囲弾強化",
        [](Player* p) {
            if (!p->HasOrbitBullets()) p->AddOrbitBullets();
            else p->UpgradeOrbitBullets();
        },
        TextureManager::Load("lvup_orbit.png")
        });

    levelUpOptions_.push_back({
        "攻撃力 +1",
        [](Player* p) { p->UpgradeAttackPower(); },
        TextureManager::Load("lvup_attack.png")
        });

    levelUpOptions_.push_back({
        "HP回復",
        [](Player* p) { p->RecoverHP(); },
        TextureManager::Load("lvup_heal.png")
        });
}

void GameScene::Update() {
    // フェード更新（最優先で処理）
    fade_.Update();

    // --- 背景更新 ---
    if (gridPlane_) { gridPlane_->Update(); }
    if (skyDome_) { skyDome_->Update(); }

    // --- 開始演出処理（Ready → Go → Play） ---
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

    // --- ポーズ処理（ESCキーで切り替え、フェード中は無効） ---
    if (input_->TriggerKey(DIK_ESCAPE) && fade_.GetState() == Fade::State::kStay) {
        paused_ = !paused_;
        Audio::GetInstance()->PlayWave(pauseSEHandle_, false, 0.5f);
    }

    if (paused_) {
        // ポーズ中のシーン遷移選択
        if (input_->TriggerKey(DIK_SPACE)) {
            fade_.StartFadeOut();
            fadeOutStarted_ = true;
            SetSceneNo(Scene::Result);
        }

        if (fadeOutStarted_ && fade_.IsFinished()) {
            GameData::totalExp = player_->GetTotalEXP();
            GameData::finalLevel = player_->GetLevel();
            finished_ = true;
        }
        return;
    }

    // --- レベルアップ開始 ---
    if (player_->IsLevelUpRequested()) {

        currentChoices_.clear();

        std::vector<int> indices(levelUpOptions_.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), std::mt19937(std::random_device{}()));

        for (int i = 0; i < 3; i++) {
            currentChoices_.push_back(levelUpOptions_[indices[i]]);
        }

        // ★ スプライト生成
        for (int i = 0; i < 3; i++) {
            choiceSprite_[i] = std::unique_ptr<Sprite>(
                Sprite::Create(currentChoices_[i].textureHandle, { 0.0f, 0.0f + static_cast<float>(i) * 140.0f })
            );
            choiceSprite_[i]->SetSize({ 1280, 720 });
        }

        levelUpActive_ = true;
        levelUpSelection_ = 0;
        player_->ClearLevelUpRequest();
        return;
    }
    if (levelUpActive_) {

        if (input_->TriggerKey(DIK_W)) {
            levelUpSelection_ = std::max<int32_t>(0, levelUpSelection_ - 1);
        }
        else if (input_->TriggerKey(DIK_S)) {
            levelUpSelection_ = std::min<int32_t>(2, levelUpSelection_ + 1);
        }

        // 矢印位置
        arrowSprite_->SetPosition({ 0.0f, 0.0f + levelUpSelection_ * 140 });

        if (input_->TriggerKey(DIK_RETURN) || input_->TriggerKey(DIK_SPACE)) {

            // ★ 選択されたアクションを実行
            currentChoices_[levelUpSelection_].action(player_.get());

            levelUpActive_ = false;
        }

        return;
    }

    // --- Wave管理（全敵撃破判定） ---
    bool allEnemiesDefeated = true;
    for (auto& enemy : enemyManager_.GetEnemies()) {
        if (enemy->IsActive()) {
            allEnemiesDefeated = false;
            break;
        }
    }

    if (allEnemiesDefeated && !waveLoading_) {
        static constexpr int32_t kMaxWave = 3;

        if (currentWave_ >= kMaxWave) {
            // 最終Wave終了 → ResultSceneへ遷移
            if (!fadeOutStarted_) {
                fade_.StartFadeOut();
                fadeOutStarted_ = true;
                SetSceneNo(Scene::Result);
            }
            if (fadeOutStarted_ && fade_.IsFinished()) {
                GameData::totalExp = player_->GetTotalEXP();
                GameData::finalLevel = player_->GetLevel();
                finished_ = true;
            }
            return;
        }

        // 次Wave読み込み
        waveLoading_ = true;
        currentWave_++;
        std::string nextCSV = "Resources/csv/wave" + std::to_string(currentWave_) + ".csv";
        enemyManager_.Initialize(nextCSV, player_.get());
        player_->SetEnemyManager(&enemyManager_);
        waveLoading_ = false;
    }

    // --- UI更新 ---
    if (hpGauge_) {
        hpGauge_->SetHP(player_->GetHP(), player_->GetMaxHP());
        hpGauge_->Update();
    }
    if (waveUI_) {
        waveUI_->SetWave(currentWave_);
        waveUI_->Update();
    }

    // --- プレイヤー死亡演出 ---
    if (player_->IsDead() && hpGauge_->IsDepleted() && !deathFadeInStarted_) {
        deathFadeInStarted_ = true;
        deathAlpha_ = 0.0f;
        gameStopped_ = true;
    }
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
            SetSceneNo(Scene::Result);
        }
        if (fadeOutStarted_ && fade_.IsFinished()) {
            GameData::totalExp = player_->GetTotalEXP();
            GameData::finalLevel = player_->GetLevel();  // ★追加
            finished_ = true;
        }
        return;
    }

    // --- 経験値ゲージ更新 ---
    if (expGauge_) {
        expGauge_->SetEXP(player_->GetEXP(), player_->GetNextLevelEXP());
        expGauge_->SetLevel(player_->GetLevel());
        expGauge_->Update();
    }

    // --- プレイヤーと敵の更新 ---
    player_->Update();
    enemyManager_.Update();

    // --- 当たり判定（EnemyManager に委譲） --- 
    enemyManager_.CheckCollisions(player_.get());

    auto setKeyColor = [&](Sprite* key, bool pressed) {
        if (pressed) {
            // 押している間は黄色
            key->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
        }
        else {
            // 押していないときは通常色（白）
            key->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        }
        };

    setKeyColor(keyW_.get(), input_->PushKey(DIK_W));
    setKeyColor(keyA_.get(), input_->PushKey(DIK_A));
    setKeyColor(keyS_.get(), input_->PushKey(DIK_S));
    setKeyColor(keyD_.get(), input_->PushKey(DIK_D));
}

void GameScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // --- スプライト描画の前処理（背景やUIを描画する準備） ---
    Sprite::PreDraw(dxCommon->GetCommandList());
    Sprite::PostDraw();

    // --- モデル描画の前処理（3Dオブジェクトを描画する準備） ---
    dxCommon_->ClearDepthBuffer();
    Model::PreDraw();

    // --- 背景要素の描画 ---
    if (gridPlane_) {
        gridPlane_->Draw(&player_->GetCamera()); // 床グリッド
    }
    if (skyDome_) {
        skyDome_->Draw(); // 天球背景
    }

    // --- プレイヤー描画 ---
    if (player_) {
        player_->Draw();
    }

    // --- 敵描画（Ready/Go中は非表示、Play開始後のみ表示） ---
    if (startState_ == StartState::Play) {
        enemyManager_.Draw(&player_->GetCamera());
    }

    enemyManager_.DrawHitParticles(&player_->GetCamera());

    // --- モデル描画の後処理 ---
    Model::PostDraw();

    // --- スプライト描画の前処理（UIや演出を描画する準備） ---
    Sprite::PreDraw(dxCommon->GetCommandList());

    ESC_ui_->Draw();

    // --- 死亡演出中のオーバーレイ描画 ---
    if (deathFadeInStarted_ && deathOverlay_) {
        deathOverlay_->Draw();
    }

    // --- スタート演出（Ready/Go）のオーバーレイ描画 ---
    if (startState_ == StartState::Ready && readyOverlay_) {
        readyOverlay_->Draw();
    }
    else if (startState_ == StartState::Go && goOverlay_) {
        goOverlay_->Draw();
    }

    // --- ガイド表示 ---
	if (startState_ == StartState::Play && guide_) {
		//guide_->Draw();

        keyW_->Draw();
        keyA_->Draw();
        keyS_->Draw();
        keyD_->Draw();
	}

    // --- レベルアップ選択画面の描画 ---
    if (levelUpActive_) {

        // 背景
        levelUpOverlay_->Draw();

        // ★ 選択肢スプライト描画
        for (int i = 0; i < 3; i++) {
            if (choiceSprite_[i]) {
                choiceSprite_[i]->Draw();
            }
        }

        // 矢印
        arrowSprite_->Draw();
    }

    // --- 経験値ゲージ描画 ---
    if (expGauge_) {
        expGauge_->Draw();
    }

    // --- HPゲージ描画（プレイヤーが生存中かつレベルアップ・ポーズ中でない場合のみ） ---
    if (hpGauge_ && player_ && !player_->IsDead() && !levelUpActive_ && !paused_) {
        hpGauge_->Draw();
    }

    // --- WaveUI描画（現在のWaveを表示） ---
    if (waveUI_) {
        waveUI_->Draw();
    }

    // --- ポーズ中のオーバーレイ描画 ---
    if (paused_ && pauseOverlay_) {
        Sprite::PreDraw(dxCommon->GetCommandList());
        pauseOverlay_->Draw();

        // ミニマップの中心（画面中央）
        Vector2 mapCenter = { 640, 360 };

        // ミニマップの半サイズ（200×200 を想定）
        float mapHalf = 180.0f;

        // ミニマップの縮尺（広さ調整）
        float scale = 3.0f; // 縮尺（ワールド距離 → ミニマップ距離）

        // プレイヤー位置
        Vector3 pPos = player_->GetWorldPosition();

        // --- 敵アイコン描画（複数対応） ---
        for (auto& e : enemyManager_.GetEnemies()) {
            if (!e->IsActive()) continue;

            Vector3 ePos = e->GetPosition();
            Vector3 rel = { ePos.x - pPos.x, 0, ePos.z - pPos.z };

            float mx = rel.x * scale;
            float my = -rel.z * scale;

            // ★ 四角形ミニマップの端に固定
            if (mx > mapHalf) mx = mapHalf;
            if (mx < -mapHalf) mx = -mapHalf;
            if (my > mapHalf) my = mapHalf;
            if (my < -mapHalf) my = -mapHalf;

            Vector2 drawPos = {
                mapCenter.x + mx,
                mapCenter.y + my
            };

            auto icon = Sprite::Create(TextureManager::Load("minimap_enemy.png"), drawPos);
            icon->SetSize({ 20, 20 });
            icon->Draw();
        }

        // --- 経験値オーブ描画（複数対応） ---
        for (auto& orb : enemyManager_.GetExpOrbs()) {
            if (!orb->IsActive()) continue;

            Vector3 oPos = orb->GetPosition();
            Vector3 rel = { oPos.x - pPos.x, 0, oPos.z - pPos.z };

            float mx = rel.x * scale;
            float my = -rel.z * scale;

            // ★ 四角形ミニマップの端に固定
            if (mx > mapHalf) mx = mapHalf;
            if (mx < -mapHalf) mx = -mapHalf;
            if (my > mapHalf) my = mapHalf;
            if (my < -mapHalf) my = -mapHalf;

            Vector2 drawPos = {
                mapCenter.x + mx,
                mapCenter.y + my
            };

            auto icon = Sprite::Create(TextureManager::Load("minimap_orb.png"), drawPos);
            icon->SetSize({ 16, 16 });
            icon->Draw();
        }
        Sprite::PostDraw();
    }

    // --- フェード演出描画（シーン遷移時の暗転） ---
    fade_.Draw();

    // --- スプライト描画の後処理 ---
    Sprite::PostDraw();
}

void GameScene::Finalize() {
    // オーディオ停止（必要に応じて）

    // フェード演出のリセット
    fadeOutStarted_ = false;
    finished_ = false;
    paused_ = false;
    deathFadeInStarted_ = false;
    deathFadeInComplete_ = false;
    gameStopped_ = false;
    levelUpActive_ = false;

    // Wave情報リセット
    currentWave_ = 1;
    waveLoading_ = false;
}
