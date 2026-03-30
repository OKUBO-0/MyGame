#include "GameScene.h"
#include "../../core/InputBindings.h"
using namespace KamataEngine;

namespace DirectXGame {

void GameScene::Initialize() {
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    InitializeAudio();
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
}

void GameScene::InitializeUI() {
    startController_.Initialize();

    uint32_t deathTex = TextureManager::Load("ui/game/death.png");
    deathOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(deathTex, { 0, 0 }));
    deathOverlay_->SetSize({ 1280, 720 });
    deathOverlay_->SetColor({ 1, 1, 1, 0.0f });

    keyUI_ = std::make_unique<KeyUI>();
    keyUI_->Initialize();

    expGauge_ = std::make_unique<ExpGauge>();
    expGauge_->Initialize();

    hpGauge_ = std::make_unique<HpGauge>();
    hpGauge_->Initialize();

    pauseController_.Initialize();

    timer_ = std::make_unique<Timer>();
    timer_->Initialize();

    levelUpController_.Initialize();
    levelUpController_.RegisterDefaultOptions();
}

void GameScene::Update(float deltaTime) {
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

    if (UpdateStartWaiting()) {
        DrawDebugUI();
        return;
    }
    if (UpdatePauseState()) {
        DrawDebugUI();
        return;
    }
    if (UpdateLevelUpFlow()) {
        DrawDebugUI();
        return;
    }
    if (UpdateGameTimer(deltaTime)) {
        DrawDebugUI();
        return;
    }

    UpdateStatusUI();

    if (UpdateDeathFlow(deltaTime)) {
        DrawDebugUI();
        return;
    }

    UpdateGameplay(deltaTime);
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

    return FinalizeResultTransition() || true;
}

bool GameScene::UpdateLevelUpFlow() {
    if (levelUpController_.TryStart(playerManager_.get(), audio_, levelUpSEHandle_)) {
        return true;
    }

    return levelUpController_.Update(playerManager_.get(), input_, audio_, pauseSEHandle_, startSEHandle_);
}

bool GameScene::UpdateGameTimer(float deltaTime) {
    if (startController_.IsWaiting() || pauseController_.IsActive() || levelUpController_.IsActive() || gameStopped_) {
        return false;
    }

    timer_->Update(deltaTime);
    gameTime_ += deltaTime;

    if (gameTime_ < gameTimeLimit_) {
        return false;
    }

    StartResultTransition();
    return FinalizeResultTransition() || true;
}

void GameScene::UpdateStatusUI() {
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
    if (playerManager_->IsDead() && hpGauge_->IsDepleted() && !deathFadeInStarted_) {
        deathFadeInStarted_ = true;
        deathAlpha_ = 0.0f;
        gameStopped_ = true;
        Audio::GetInstance()->PlayWave(deathSEHandle_, false, 1.0f);
    }

    if (!gameStopped_) {
        return false;
    }

    if (deathFadeInStarted_ && !deathFadeInComplete_) {
        deathAlpha_ += deltaTime * kDeathFadeSpeedPerSecond_;
        if (deathAlpha_ >= 0.5f) {
            deathAlpha_ = 0.5f;
            deathFadeInComplete_ = true;
        }
        deathOverlay_->SetColor({ 1, 1, 1, deathAlpha_ });
    }

    if (deathFadeInComplete_ &&
        InputBindings::IsConfirmTriggered(input_) &&
        curtain_.GetState() == CurtainTransition::State::kNone) {
        StartResultTransition();
    }

    return FinalizeResultTransition() || true;
}

bool GameScene::FinalizeResultTransition() {
    if (!curtainCloseStarted_ || !curtain_.IsFinished()) {
        return false;
    }

    if (sessionContext_) {
        sessionContext_->resultData.totalExp = playerManager_->GetTotalEXP();
        sessionContext_->resultData.finalLevel = playerManager_->GetLevel();
        sessionContext_->resultData.totalKillCount = enemyManager_.GetTotalKillCount();
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
    if (gridPlane_) {
        gridPlane_->Draw(&player_->GetCamera());
    }
    if (skyDome_) {
        skyDome_->Draw();
    }
    if (player_) {
        player_->Draw();
    }
    if (!startController_.IsWaiting()) {
        enemyManager_.Draw(&player_->GetCamera());
    }

    enemyManager_.DrawHitParticles(&player_->GetCamera());
    playerManager_->Draw(&player_->GetCamera());
}

void GameScene::DrawUI() {
    startController_.Draw();

    if (!startController_.IsWaiting()) {
        keyUI_->Draw();
    }

    if (deathFadeInStarted_ && deathOverlay_) {
        deathOverlay_->Draw();
    }

    levelUpController_.Draw();

    if (expGauge_) {
        expGauge_->Draw();
    }

    if (hpGauge_ && player_ && !playerManager_->IsDead() && !levelUpController_.IsActive() &&
        !pauseController_.IsActive() && !startController_.IsWaiting()) {
        hpGauge_->Draw();
    }

    if (timer_) {
        timer_->Draw();
    }

    pauseController_.Draw();

    curtain_.Draw();
}

void GameScene::Finalize() {
    finished_ = false;
    curtainCloseStarted_ = false;
    deathFadeInStarted_ = false;
    deathFadeInComplete_ = false;
    gameStopped_ = false;
    startController_.Reset();
    pauseController_.Reset();
    levelUpController_.Reset();
}

} // namespace DirectXGame
