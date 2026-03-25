#include "GamePauseController.h"
#include "EnemyManager.h"
#include "Player.h"
#include <KamataEngine.h>

using namespace KamataEngine;

void GamePauseController::Initialize() {
    audio_ = Audio::GetInstance();

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

    selectSEHandle_ = audio_->LoadWave("audio/se/se_pause.wav");
    decideSEHandle_ = audio_->LoadWave("audio/se/se_exp.wav");

    active_ = false;
    guideActive_ = false;
    goResult_ = false;
    menuIndex_ = 0;
}

bool GamePauseController::Update(Player* player, const EnemyManager& enemyManager, Input* input,
                                 Audio* audio, uint32_t toggleSEHandle) {
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

void GamePauseController::Draw(DirectXCommon* dxCommon) const {
    if (!active_) {
        return;
    }

    Sprite::PreDraw(dxCommon->GetCommandList());
    pauseOverlay_->Draw();
    miniMap_->Draw();
    if (guideActive_) {
        guideSprite_->Draw();
    } else {
        cursorSprite_->Draw();
    }
    Sprite::PostDraw();
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
