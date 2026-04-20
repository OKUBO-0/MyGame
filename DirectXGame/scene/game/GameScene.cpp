#include "GameScene.h"
#include "../../core/InputBindings.h"
#include "../../core/ModelCache.h"
#include "../../core/ScreenUtil.h"
using namespace KamataEngine;

namespace DirectXGame {

void GameScene::Initialize() {
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    InitializeAudio();
    InitializeLighting();
    InitializeSceneObjects();
    InitializeUI();

    if (sessionContext_) {
        sessionContext_->resultData = {};
    }
}

void GameScene::InitializeAudio() {
    pauseSEHandle_ = audio_->LoadWave("audio/se/se_pause.wav");
    startSEHandle_ = audio_->LoadWave("audio/se/se_exp.wav");
    deathSEHandle_ = audio_->LoadWave("audio/se/se_death.wav");
    levelUpSEHandle_ = audio_->LoadWave("audio/se/se_exp.wav");
}

void GameScene::InitializeLighting() {
    lightGroup_.reset(LightGroup::Create());
    if (!lightGroup_) {
        return;
    }

    lightGroup_->SetAmbientColor({0.48f, 0.48f, 0.52f});
    lightGroup_->SetDirLightActive(0, true);
    lightGroup_->SetDirLightDir(0, {-0.35f, -1.0f, -0.4f});
    lightGroup_->SetDirLightColor(0, {1.05f, 1.0f, 0.95f});
    lightGroup_->SetDirLightActive(1, false);
    lightGroup_->SetDirLightActive(2, false);
    lightGroup_->Update();
}

void GameScene::InitializeSceneObjects() {
    curtain_.Initialize();
    curtain_.StartOpen(20.0f);
    curtainOpening_ = true;

    player_ = std::make_unique<Player>();
    player_->Initialize();

    playerManager_ = std::make_unique<PlayerManager>();
    playerManager_->Initialize(player_.get());
    playerManager_->LoadStatusFromCSV("Resources/data/playerStatus.csv");

    enemyManager_.Initialize("Resources/data/enemyTypes.csv", player_.get(), playerManager_.get());

    gridPlane_ = std::make_unique<GridPlane>();
    gridPlane_->Initialize();

    skyDome_ = std::make_unique<SkyDome>();
    skyDome_->Initialize();

    ApplyLighting();
}

void GameScene::InitializeUI() {
    InitializeHudUI();
    InitializeOverlayUI();
}

void GameScene::InitializeHudUI() {
    keyUI_ = std::make_unique<KeyUI>();
    keyUI_->Initialize();

    expGauge_ = std::make_unique<ExpGauge>();
    expGauge_->Initialize();

    hpGauge_ = std::make_unique<HpGauge>();
    hpGauge_->Initialize();

    timer_ = std::make_unique<Timer>();
    timer_->Initialize();
}

void GameScene::InitializeOverlayUI() {
    startController_.Initialize();

    uint32_t deathTex = TextureManager::Load("ui/game/death.png");
    deathOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(deathTex, { 0, 0 }));
    deathOverlay_->SetSize(ScreenUtil::GetClientSize());
    deathOverlay_->SetColor({ 1, 1, 1, 0.0f });

    pauseController_.Initialize();

    levelUpController_.Initialize();
    levelUpController_.RegisterDefaultOptions();
}

void GameScene::ApplyLighting() {
    if (!lightGroup_) {
        return;
    }

    if (player_) {
        player_->SetLightGroup(lightGroup_.get());
    }
    if (gridPlane_) {
        gridPlane_->SetLightGroup(lightGroup_.get());
    }
    if (skyDome_) {
        skyDome_->SetLightGroup(lightGroup_.get());
    }

    ModelCache::Get("octopus")->SetLightGroup(lightGroup_.get());
    ModelCache::Get("bullet")->SetLightGroup(lightGroup_.get());
    ModelCache::Get("cube")->SetLightGroup(lightGroup_.get());
    ModelCache::Get("ripples")->SetLightGroup(lightGroup_.get());
    ModelCache::Get("ExpOrb")->SetLightGroup(lightGroup_.get());
    ModelCache::Get("Enemy1")->SetLightGroup(lightGroup_.get());
    ModelCache::Get("Enemy2")->SetLightGroup(lightGroup_.get());
    ModelCache::Get("Enemy3")->SetLightGroup(lightGroup_.get());
    ModelCache::Get("Enemy4")->SetLightGroup(lightGroup_.get());
}

GameScene::GameFlowState GameScene::ResolveFlowState() const {
    if (curtainCloseStarted_) {
        return GameFlowState::ResultTransition;
    }
    if (curtainOpening_) {
        return GameFlowState::Opening;
    }
    if (startController_.IsWaiting()) {
        return GameFlowState::StartWaiting;
    }
    if (pauseController_.IsActive()) {
        return GameFlowState::Pause;
    }
    if (levelUpController_.IsActive() || (playerManager_ && playerManager_->IsLevelUpRequested())) {
        return GameFlowState::LevelUp;
    }
    if (deathState_ != DeathState::None) {
        return GameFlowState::Death;
    }
    return GameFlowState::Playing;
}

bool GameScene::ShouldDrawGameplayUI(GameFlowState flowState) const {
    return flowState != GameFlowState::StartWaiting;
}

bool GameScene::ShouldDrawHpGauge(GameFlowState flowState) const {
    return hpGauge_ && player_ && !playerManager_->IsDead() && flowState == GameFlowState::Playing;
}

bool GameScene::ShouldDrawEnemies(GameFlowState flowState) const {
    return flowState != GameFlowState::StartWaiting;
}

void GameScene::Update(float deltaTime) {
    if (lightGroup_) {
        lightGroup_->Update();
    }
    curtain_.Update(deltaTime);

    if (FinalizeResultTransition()) {
        DrawDebugUI();
        return;
    }

    if (UpdateCurtainOpening()) {
        DrawDebugUI();
        return;
    }

    if (gridPlane_) {
        gridPlane_->Update(player_->GetWorldPosition());
    }
    if (skyDome_) {
        skyDome_->Update();
    }

    switch (ResolveFlowState()) {
    case GameFlowState::Opening:
        UpdateCurtainOpening();
        break;

    case GameFlowState::StartWaiting:
        UpdateStartWaiting();
        break;

    case GameFlowState::Pause:
        UpdatePauseState();
        break;

    case GameFlowState::LevelUp:
        UpdateLevelUpFlow(deltaTime);
        break;

    case GameFlowState::Death:
        UpdateStatusUI();
        UpdateDeathFlow(deltaTime);
        break;

    case GameFlowState::Playing:
        if (UpdatePauseState()) {
            break;
        }
        if (!UpdateGameTimer(deltaTime)) {
            UpdateStatusUI();
            if (!UpdateDeathFlow(deltaTime)) {
                UpdateGameplay(deltaTime);
            }
        }
        break;

    case GameFlowState::ResultTransition:
        break;
    }
    DrawDebugUI();
}

bool GameScene::UpdateCurtainOpening() {
    if (!curtainOpening_) {
        return false;
    }

    if (curtain_.GetState() == CurtainTransition::State::kNone) {
        curtainOpening_ = false;
        return false;
    }

    return true;
}

bool GameScene::UpdateStartWaiting() {
    return startController_.Update(input_, audio_, startSEHandle_);
}

bool GameScene::UpdatePauseState() {
    if (curtain_.GetState() != CurtainTransition::State::kNone) {
        return false;
    }

    if (!pauseController_.Update(player_.get(), enemyManager_, *playerManager_, input_, audio_, pauseSEHandle_)) {
        return false;
    }

    if (pauseController_.ShouldGoResult()) {
        StartResultTransition();
    }

    return true;
}

bool GameScene::UpdateLevelUpFlow(float deltaTime) {
    if (levelUpController_.TryStart(playerManager_.get(), audio_, levelUpSEHandle_)) {
        return true;
    }

    return levelUpController_.Update(playerManager_.get(), input_, audio_, pauseSEHandle_, startSEHandle_, deltaTime);
}

bool GameScene::UpdateGameTimer(float deltaTime) {
    if (ResolveFlowState() != GameFlowState::Playing) {
        return false;
    }

    timer_->Update(deltaTime);
    gameTime_ += deltaTime;

    if (gameTime_ < gameTimeLimit_) {
        return false;
    }

    StartResultTransition();
    return true;
}

void GameScene::UpdateStatusUI() {
    UpdateHud();
}

void GameScene::UpdateHud() {
    if (hpGauge_) {
        hpGauge_->SetHP(playerManager_->GetHP(), playerManager_->GetMaxHP());
        hpGauge_->Update();
    }

    if (expGauge_) {
        expGauge_->SetEXP(playerManager_->GetEXP(), playerManager_->GetNextLevelEXP());
        expGauge_->SetLevel(playerManager_->GetLevel());
        expGauge_->Update();
    }
}

bool GameScene::UpdateDeathFlow(float deltaTime) {
    if (playerManager_->IsDead() && hpGauge_->IsDepleted() && deathState_ == DeathState::None) {
        deathState_ = DeathState::FadingIn;
        deathAlpha_ = 0.0f;
        Audio::GetInstance()->PlayWave(deathSEHandle_, false, 1.0f);
    }

    if (deathState_ == DeathState::None) {
        return false;
    }

    if (deathState_ == DeathState::FadingIn) {
        deathAlpha_ += deltaTime * kDeathFadeSpeedPerSecond_;
        if (deathAlpha_ >= 0.5f) {
            deathAlpha_ = 0.5f;
            deathState_ = DeathState::WaitingConfirm;
        }
        deathOverlay_->SetColor({ 1, 1, 1, deathAlpha_ });
    }

    if (deathState_ == DeathState::WaitingConfirm &&
        InputBindings::IsUiConfirmTriggered(input_) &&
        curtain_.GetState() == CurtainTransition::State::kNone) {
        StartResultTransition();
    }

    return true;
}

bool GameScene::FinalizeResultTransition() {
    if (!curtainCloseStarted_ || !curtain_.IsFinished()) {
        return false;
    }

    if (sessionContext_) {
        const int32_t totalExp = playerManager_->GetTotalEXP();
        const int32_t finalLevel = playerManager_->GetLevel();
        const int32_t totalKillCount = enemyManager_.GetTotalKillCount();
        sessionContext_->resultData.totalExp = totalExp;
        sessionContext_->resultData.finalLevel = finalLevel;
        sessionContext_->resultData.totalKillCount = totalKillCount;
    }
    finished_ = true;
    return true;
}

void GameScene::StartResultTransition() {
    if (curtain_.GetState() != CurtainTransition::State::kNone) {
        return;
    }

    curtain_.StartClose();
    curtainCloseStarted_ = true;
    SetSceneNo(Scene::Result);
}

void GameScene::UpdateGameplay(float deltaTime) {
    if (input_ && input_->TriggerKey(DIK_F9)) {
        playerManager_->MaxAllWeapons();
    }

    player_->Update(deltaTime);
    playerManager_->Update(deltaTime);
    enemyManager_.Update(deltaTime);
    enemyManager_.CheckCollisions(player_.get(), playerManager_.get());

    if (!startController_.IsWaiting()) {
        keyUI_->Update(input_);
    }
}

void GameScene::DrawDebugUI() {
#ifdef _DEBUG
    pauseController_.DebugDrawImGui();
    levelUpController_.DebugDrawImGui();
    if (ImGui::Begin("UI Debug")) {
        if (expGauge_) {
            expGauge_->DebugDrawImGui();
        }
        if (hpGauge_) {
            hpGauge_->DebugDrawImGui();
        }
        if (timer_) {
            timer_->DebugDrawImGui();
        }
    }
    ImGui::End();
#endif
}

void GameScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    Sprite::PreDraw(dxCommon->GetCommandList());
    Sprite::PostDraw();

    dxCommon_->ClearDepthBuffer();
    Model::PreDraw();
    DrawWorld();
    Model::PostDraw();

    Sprite::PreDraw(dxCommon->GetCommandList());
    DrawUI();
    Sprite::PostDraw();
}

void GameScene::DrawWorld() {
    const GameFlowState flowState = ResolveFlowState();

    if (gridPlane_) {
        gridPlane_->Draw(&player_->GetCamera());
    }
    if (skyDome_) {
        skyDome_->Draw();
    }
    if (player_) {
        player_->Draw();
    }
    if (ShouldDrawEnemies(flowState)) {
        enemyManager_.Draw(&player_->GetCamera());
    }

    enemyManager_.DrawHitParticles(&player_->GetCamera());
    playerManager_->Draw(&player_->GetCamera());
}

void GameScene::DrawHud(GameFlowState flowState) {
    if (ShouldDrawGameplayUI(flowState)) {
        keyUI_->Draw();
    }

    if (expGauge_) {
        expGauge_->Draw();
    }

    if (ShouldDrawHpGauge(flowState)) {
        hpGauge_->Draw();
    }

    if (timer_) {
        timer_->Draw();
    }
}

void GameScene::DrawOverlayUI(GameFlowState) {
    startController_.Draw();

    if (deathState_ != DeathState::None && deathOverlay_) {
        deathOverlay_->Draw();
    }

    levelUpController_.Draw();

    pauseController_.Draw();

    curtain_.Draw();
}

void GameScene::DrawUI() {
    const GameFlowState flowState = ResolveFlowState();
    DrawHud(flowState);
    DrawOverlayUI(flowState);
}

void GameScene::Finalize() {
    finished_ = false;
    curtainCloseStarted_ = false;
    deathState_ = DeathState::None;
    deathAlpha_ = 0.0f;
    startController_.Reset();
    pauseController_.Reset();
    levelUpController_.Reset();
}

} // namespace DirectXGame
