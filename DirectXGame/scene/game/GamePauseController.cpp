#include "GamePauseController.h"
#include "EnemyManager.h"
#include "../../player/core/PlayerManager.h"
#include "Player.h"
#include <algorithm>
#include <KamataEngine.h>

using namespace KamataEngine;

namespace DirectXGame {

void GamePauseController::Initialize() {
    audio_ = Audio::GetInstance();
    const uint32_t whiteTexture = TextureManager::Load("textures/debug/white1x1.png");

    uint32_t pauseTexture = TextureManager::Load("ui/game/pause.png");
    pauseOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(pauseTexture, {0, 0}));
    pauseOverlay_->SetSize({1280, 720});

    uint32_t guideTexture = TextureManager::Load("ui/title/guideUI.png");
    guideSprite_ = std::unique_ptr<Sprite>(Sprite::Create(guideTexture, {0, 0}));
    guideSprite_->SetSize({1280, 720});

    uint32_t cursorTexture = TextureManager::Load("ui/game/pause_arrow.png");
    cursorSprite_ = std::unique_ptr<Sprite>(Sprite::Create(cursorTexture, {0, 0}));
    cursorSprite_->SetSize({1280, 720});

    miniMap_ = std::make_unique<MiniMap>();
    miniMap_->Initialize();

    statsPanel_ = std::unique_ptr<Sprite>(Sprite::Create(whiteTexture, {760, 450}));
    statsPanel_->SetSize({420, 180});
    statsPanel_->SetColor({0.0f, 0.0f, 0.0f, 0.72f});

    killIconSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/game/minimap_enemy.png"), {790, 440}));
    killIconSprite_->SetSize({28, 28});

    buildNormalSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/game/lvup_normal.png"), {782, 510}));
    buildNormalSprite_->SetSize({92, 52});

    buildOrbitSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/game/lvup_orbit_add.png"), {882, 510}));
    buildOrbitSprite_->SetSize({92, 52});

    buildDroneSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/game/lvup_drone_add.png"), {982, 510}));
    buildDroneSprite_->SetSize({92, 52});

    buildAttackSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/game/lvup_attack.png"), {1082, 510}));
    buildAttackSprite_->SetSize({92, 52});

    killScore_ = std::make_unique<Score>();
    killScore_->Initialize();
    killScore_->SetPosition({832, 438});
    killScore_->SetScale(1.0f);

    selectSEHandle_ = audio_->LoadWave("audio/se/se_pause.wav");
    decideSEHandle_ = audio_->LoadWave("audio/se/se_exp.wav");

    active_ = false;
    guideActive_ = false;
    goResult_ = false;
    menuIndex_ = 0;
}

bool GamePauseController::Update(Player* player, const EnemyManager& enemyManager, const PlayerManager& playerManager,
                                 Input* input, Audio* audio, uint32_t toggleSEHandle) {
    if (!guideActive_ && input->TriggerKey(DIK_ESCAPE)) {
        active_ = !active_;
        guideActive_ = false;
        goResult_ = false;
        if (toggleSEHandle != 0) {
            audio->PlayWave(toggleSEHandle, false, 0.5f);
        }
    }

    if (!active_) {
        return false;
    }

    if (guideActive_) {
        if (input->TriggerKey(DIK_ESCAPE)) {
            guideActive_ = false;
            if (decideSEHandle_ != 0) {
                audio_->PlayWave(decideSEHandle_, false, 1.0f);
            }
        }
        return true;
    }

    miniMap_->Update(player, enemyManager);
    UpdateStats(enemyManager, playerManager);

    int32_t previousMenuIndex = menuIndex_;
    if (input->TriggerKey(DIK_W)) {
        menuIndex_ = std::max<int32_t>(0, menuIndex_ - 1);
    }
    if (input->TriggerKey(DIK_S)) {
        menuIndex_ = std::min<int32_t>(1, menuIndex_ + 1);
    }
    if (menuIndex_ != previousMenuIndex && selectSEHandle_ != 0) {
        audio_->PlayWave(selectSEHandle_, false, 1.0f);
    }

    switch (menuIndex_) {
    case 0:
        cursorSprite_->SetPosition({0, 0});
        break;
    case 1:
        cursorSprite_->SetPosition({0, 170});
        break;
    }

    if (input->TriggerKey(DIK_SPACE) || input->TriggerKey(DIK_RETURN)) {
        if (decideSEHandle_ != 0) {
            audio_->PlayWave(decideSEHandle_, false, 1.0f);
        }
        if (menuIndex_ == 0) {
            guideActive_ = true;
        } else if (menuIndex_ == 1) {
            goResult_ = true;
        }
    }

    return true;
}

void GamePauseController::Draw() const {
    if (!active_) {
        return;
    }

    pauseOverlay_->Draw();
    miniMap_->Draw();
    if (guideActive_) {
        guideSprite_->Draw();
    } else {
        statsPanel_->Draw();
        killIconSprite_->Draw();
        killScore_->Draw();
        buildNormalSprite_->Draw();
        buildOrbitSprite_->Draw();
        buildDroneSprite_->Draw();
        buildAttackSprite_->Draw();
        cursorSprite_->Draw();
    }
}

void GamePauseController::Reset() {
    active_ = false;
    guideActive_ = false;
    goResult_ = false;
    menuIndex_ = 0;
}

bool GamePauseController::IsActive() const {
    return active_;
}

bool GamePauseController::IsGuideActive() const {
    return guideActive_;
}

bool GamePauseController::ShouldGoResult() const {
    return goResult_;
}

void GamePauseController::UpdateStats(const EnemyManager& enemyManager, const PlayerManager& playerManager) {
    killScore_->SetNumber(enemyManager.GetTotalKillCount());

    buildNormalSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
    buildOrbitSprite_->SetColor({1.0f, 1.0f, 1.0f, playerManager.HasOrbitBullets() ? 1.0f : 0.25f});
    buildDroneSprite_->SetColor({1.0f, 1.0f, 1.0f, playerManager.HasDrone() ? 1.0f : 0.25f});
    buildAttackSprite_->SetColor({1.0f, 1.0f, 1.0f, playerManager.GetAttackPower() > 1 ? 1.0f : 0.4f});
}

} // namespace DirectXGame
