#include "GameScene.h"
#include <random>
using namespace KamataEngine;

void GameScene::Initialize() {
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    pauseSEHandle_ = Audio::GetInstance()->LoadWave("Sounds/se_pause.wav");

    camera_.Initialize();

    uint32_t readyTex = TextureManager::Load("ready.png");
    readyOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(readyTex, { 0, 0 }));
    readyOverlay_->SetSize({ 1280, 720 });

    uint32_t goTex = TextureManager::Load("go.png");
    goOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(goTex, { 0, 0 }));
    goOverlay_->SetSize({ 1280, 720 });

    uint32_t guideTex = TextureManager::Load("guide.png");
    guide_ = std::unique_ptr<Sprite>(Sprite::Create(guideTex, { 0, 0 }));
    guide_->SetSize({ 1280, 720 });

    startState_ = StartState::Ready;
    startTimer_ = 0;

    player_ = std::make_unique<Player>();
    player_->Initialize();

    playerManager_ = std::make_unique<PlayerManager>();
    playerManager_->Initialize(player_.get());

    currentWave_ = 1;
    waveLoading_ = false;
    std::string csvPath = "Resources/csv/wave" + std::to_string(currentWave_) + ".csv";
    enemyManager_.Initialize(csvPath, player_.get(), playerManager_.get());

    fade_.Initialize();
    fadeOutStarted_ = false;

    uint32_t pauseTex = TextureManager::Load("pause.png");
    pauseOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(pauseTex, { 0, 0 }));
    pauseOverlay_->SetSize({ 1280, 720 });
    pauseOverlay_->SetColor({ 1, 1, 1, 1 });

    uint32_t deathTex = TextureManager::Load("death.png");
    deathOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(deathTex, { 0, 0 }));
    deathOverlay_->SetSize({ 1280, 720 });
    deathOverlay_->SetColor({ 1, 1, 1, 0.0f });

    uint32_t levelUpTex = TextureManager::Load("levelUp.png");
    levelUpOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(levelUpTex, { 0, 0 }));

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
    ESC_ui_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ESC_ui.png"), { 0, 0 }));

    keyW_->SetSize({ 1280, 720 });
    keyA_->SetSize({ 1280, 720 });
    keyS_->SetSize({ 1280, 720 });
    keyD_->SetSize({ 1280, 720 });
    ESC_ui_->SetSize({ 1280, 720 });

    expGauge_ = std::make_unique<ExpGauge>();
    expGauge_->Initialize();

    hpGauge_ = std::make_unique<HpGauge>();
    hpGauge_->Initialize();

    waveUI_ = std::make_unique<WaveUI>();
    waveUI_->Initialize();
    waveUI_->SetWave(currentWave_);

    gridPlane_ = std::make_unique<GridPlane>();
    gridPlane_->Initialize();

    skyDome_ = std::make_unique<SkyDome>();
    skyDome_->Initialize();

    GameData::totalExp = 0;
    GameData::finalLevel = 1;
    GameData::totalKillCount = 0;

    levelUpOptions_.push_back({
        "通常弾強化",
        [](PlayerManager* pm) { pm->UpgradeNormalBullets(); },
        TextureManager::Load("lvup_normal.png")
    });

    levelUpOptions_.push_back({
        "周囲弾強化",
        [](PlayerManager* pm) {
            if (!pm->HasOrbitBullets()) pm->AddOrbitBullets();
            else pm->UpgradeOrbitBullets();
        },
        TextureManager::Load("lvup_orbit.png")
    });

    levelUpOptions_.push_back({
        "ドローン",
        [](PlayerManager* pm) {
            if (!pm->HasDrone()) pm->AddDrone();
            else pm->UpgradeDrone();
        },
        TextureManager::Load("lvup_drone.png")
    });

    levelUpOptions_.push_back({
        "攻撃力 +1",
        [](PlayerManager* pm) { pm->UpgradeAttackPower(); },
        TextureManager::Load("lvup_attack.png")
    });

    levelUpOptions_.push_back({
        "HP回復",
        [](PlayerManager* pm) { pm->RecoverHP(); },
        TextureManager::Load("lvup_heal.png")
    });
}

void GameScene::Update() {
    fade_.Update();

    if (gridPlane_) { gridPlane_->Update(); }
    if (skyDome_) { skyDome_->Update(); }

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

    if (input_->TriggerKey(DIK_ESCAPE) && fade_.GetState() == Fade::State::kStay) {
        paused_ = !paused_;
        Audio::GetInstance()->PlayWave(pauseSEHandle_, false, 0.5f);
    }

    if (paused_) {
        if (input_->TriggerKey(DIK_SPACE) && fade_.GetState() == Fade::State::kStay) {
            fade_.StartFadeOut();
            fadeOutStarted_ = true;
            SetSceneNo(Scene::Result);
        }

        if (fadeOutStarted_ && fade_.IsFinished()) {
            GameData::totalExp = playerManager_->GetTotalEXP();
            GameData::finalLevel = playerManager_->GetLevel();
            finished_ = true;
        }
        return;
    }

    if (playerManager_->IsLevelUpRequested()) {

        currentChoices_.clear();

        std::vector<int> indices(levelUpOptions_.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), std::mt19937(std::random_device{}()));

        for (int i = 0; i < 3; i++) {
            currentChoices_.push_back(levelUpOptions_[indices[i]]);
        }

        for (int i = 0; i < 3; i++) {
            choiceSprite_[i] = std::unique_ptr<Sprite>(
                Sprite::Create(currentChoices_[i].textureHandle, { 0.0f, 0.0f + static_cast<float>(i) * 140.0f })
            );
            choiceSprite_[i]->SetSize({ 1280, 720 });
        }

        levelUpActive_ = true;
        levelUpSelection_ = 0;
        playerManager_->ClearLevelUpRequest();
        return;
    }

    if (levelUpActive_) {

        if (input_->TriggerKey(DIK_W)) {
            levelUpSelection_ = std::max<int32_t>(0, levelUpSelection_ - 1);
        }
        else if (input_->TriggerKey(DIK_S)) {
            levelUpSelection_ = std::min<int32_t>(2, levelUpSelection_ + 1);
        }

        arrowSprite_->SetPosition({ 0.0f, 0.0f + levelUpSelection_ * 140 });

        if (input_->TriggerKey(DIK_RETURN) || input_->TriggerKey(DIK_SPACE)) {
            currentChoices_[levelUpSelection_].action(playerManager_.get());
            levelUpActive_ = false;
        }

        return;
    }

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
            if (!fadeOutStarted_ && fade_.GetState() == Fade::State::kStay) {
                fade_.StartFadeOut();
                fadeOutStarted_ = true;
                SetSceneNo(Scene::Result);
            }
            if (fadeOutStarted_ && fade_.IsFinished()) {
                GameData::totalExp = playerManager_->GetTotalEXP();
                GameData::finalLevel = playerManager_->GetLevel();
                finished_ = true;
            }
            return;
        }

        waveLoading_ = true;
        currentWave_++;
        std::string nextCSV = "Resources/csv/wave" + std::to_string(currentWave_) + ".csv";
        enemyManager_.Initialize(nextCSV, player_.get(), playerManager_.get());
        waveLoading_ = false;
    }

    if (hpGauge_) {
        hpGauge_->SetHP(playerManager_->GetHP(), playerManager_->GetMaxHP());
        hpGauge_->Update();
    }

    if (waveUI_) {
        waveUI_->SetWave(currentWave_);
        waveUI_->Update();
    }

    if (playerManager_->IsDead() && hpGauge_->IsDepleted() && !deathFadeInStarted_) {
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
        if (deathFadeInComplete_ && input_->TriggerKey(DIK_SPACE) && fade_.GetState() == Fade::State::kStay) {
            fade_.StartFadeOut();
            fadeOutStarted_ = true;
            SetSceneNo(Scene::Result);
        }
        if (fadeOutStarted_ && fade_.IsFinished()) {
            GameData::totalExp = playerManager_->GetTotalEXP();
            GameData::finalLevel = playerManager_->GetLevel();
            finished_ = true;
        }
        return;
    }

    if (expGauge_) {
        expGauge_->SetEXP(playerManager_->GetEXP(), playerManager_->GetNextLevelEXP());
        expGauge_->SetLevel(playerManager_->GetLevel());
        expGauge_->Update();
    }

    player_->Update();
    playerManager_->Update();
    enemyManager_.Update();

    enemyManager_.CheckCollisions(player_.get(), playerManager_.get());

    auto setKeyColor = [&](Sprite* key, bool pressed) {
        if (pressed) {
            key->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
        }
        else {
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

    Sprite::PreDraw(dxCommon->GetCommandList());
    Sprite::PostDraw();

    dxCommon_->ClearDepthBuffer();
    Model::PreDraw();

    if (gridPlane_) {
        gridPlane_->Draw(&player_->GetCamera());
    }
    if (skyDome_) {
        skyDome_->Draw();
    }

    if (player_) {
        player_->Draw();
    }

    if (startState_ == StartState::Play) {
        enemyManager_.Draw(&player_->GetCamera());
    }

    enemyManager_.DrawHitParticles(&player_->GetCamera());
    playerManager_->Draw(&player_->GetCamera());

    Model::PostDraw();

    Sprite::PreDraw(dxCommon->GetCommandList());

    ESC_ui_->Draw();

    if (deathFadeInStarted_ && deathOverlay_) {
        deathOverlay_->Draw();
    }

    if (startState_ == StartState::Ready && readyOverlay_) {
        readyOverlay_->Draw();
    }
    else if (startState_ == StartState::Go && goOverlay_) {
        goOverlay_->Draw();
    }

    if (startState_ == StartState::Play && guide_) {
        keyW_->Draw();
        keyA_->Draw();
        keyS_->Draw();
        keyD_->Draw();
    }

    if (levelUpActive_) {
        levelUpOverlay_->Draw();

        for (int i = 0; i < 3; i++) {
            if (choiceSprite_[i]) {
                choiceSprite_[i]->Draw();
            }
        }

        arrowSprite_->Draw();
    }

    if (expGauge_) {
        expGauge_->Draw();
    }

    if (hpGauge_ && player_ && !playerManager_->IsDead() && !levelUpActive_ && !paused_) {
        hpGauge_->Draw();
    }

    if (waveUI_) {
        waveUI_->Draw();
    }

    if (paused_ && pauseOverlay_) {
        Sprite::PreDraw(dxCommon->GetCommandList());
        pauseOverlay_->Draw();

        Vector2 mapCenter = { 640, 360 };
        float mapHalf = 180.0f;
        float scale = 3.0f;

        Vector3 pPos = player_->GetWorldPosition();

        for (auto& e : enemyManager_.GetEnemies()) {
            if (!e->IsActive()) continue;

            Vector3 ePos = e->GetPosition();
            Vector3 rel = { ePos.x - pPos.x, 0, ePos.z - pPos.z };

            float mx = rel.x * scale;
            float my = -rel.z * scale;

            if (mx > mapHalf) mx = mapHalf;
            if (mx < -mapHalf) mx = -mapHalf;
            if (my > mapHalf) my = mapHalf;
            if (my < -mapHalf) my = -mapHalf;

            Vector2 drawPos = { mapCenter.x + mx, mapCenter.y + my };

            auto icon = Sprite::Create(TextureManager::Load("minimap_enemy.png"), drawPos);
            icon->SetSize({ 20, 20 });
            icon->Draw();
        }

        for (auto& orb : enemyManager_.GetExpOrbs()) {
            if (!orb->IsActive()) continue;

            Vector3 oPos = orb->GetPosition();
            Vector3 rel = { oPos.x - pPos.x, 0, oPos.z - pPos.z };

            float mx = rel.x * scale;
            float my = -rel.z * scale;

            if (mx > mapHalf) mx = mapHalf;
            if (mx < -mapHalf) mx = -mapHalf;
            if (my > mapHalf) my = mapHalf;
            if (my < -mapHalf) my = -mapHalf;

            Vector2 drawPos = { mapCenter.x + mx, mapCenter.y + my };

            auto icon = Sprite::Create(TextureManager::Load("minimap_orb.png"), drawPos);
            icon->SetSize({ 16, 16 });
            icon->Draw();
        }
        Sprite::PostDraw();
    }

    fade_.Draw();

    Sprite::PostDraw();
}

void GameScene::Finalize() {
    fadeOutStarted_ = false;
    finished_ = false;
    paused_ = false;
    deathFadeInStarted_ = false;
    deathFadeInComplete_ = false;
    gameStopped_ = false;
    levelUpActive_ = false;

    currentWave_ = 1;
    waveLoading_ = false;
}
