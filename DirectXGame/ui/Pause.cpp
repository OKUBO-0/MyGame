#include "Pause.h"
using namespace KamataEngine;

void Pause::Initialize() {
    audio_ = Audio::GetInstance();
    pauseTex_ = TextureManager::Load("pause.png");
    pauseOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(pauseTex_, { 0, 0 }));
    pauseOverlay_->SetSize({ 1280, 720 });

    guideTex_ = TextureManager::Load("title/guideUI.png");
    guideSprite_ = std::unique_ptr<Sprite>(Sprite::Create(guideTex_, { 0, 0 }));
    guideSprite_->SetSize({ 1280, 720 });

    cursorTex_ = TextureManager::Load("pause_arrow.png");
    cursorSprite_ = std::unique_ptr<Sprite>(Sprite::Create(cursorTex_, { 0, 0 }));
    cursorSprite_->SetSize({ 1280, 720 });

    miniMap_ = std::make_unique<MiniMap>();
    miniMap_->Initialize();

    selectSEHandle_ = audio_->LoadWave("Sounds/se_pause.wav");
    decideSEHandle_ = audio_->LoadWave("Sounds/se_exp.wav");
}

void Pause::Update(const Player* player, const EnemyManager& enemyManager, Input* input) {
    if (!active_) return;

    // ガイド中は ESC で閉じる
    if (guideActive_) {
        if (input->TriggerKey(DIK_ESCAPE)) {
            guideActive_ = false;
            if (decideSEHandle_ != 0) {
                audio_->PlayWave(decideSEHandle_, false, 1.0f);
            }
        }
        return;
    }

    miniMap_->Update(player, enemyManager);

    // --- メニュー操作（TitleScene と同じ方式） ---
    int32_t previousMenuIndex = menuIndex_;
    if (input->TriggerKey(DIK_W)) {
        menuIndex_ = std::max<int32_t>(0, menuIndex_ - 1);
    }
    if (input->TriggerKey(DIK_S)) {
        menuIndex_ = std::min<int32_t>(1, menuIndex_ + 1);
    }
    if (menuIndex_ != previousMenuIndex) {
        if (selectSEHandle_ != 0) {
            audio_->PlayWave(selectSEHandle_, false, 1.0f);
        }
    }

    // カーソル位置
    switch (menuIndex_) {
    case 0: cursorSprite_->SetPosition({ 0, 0 }); break;     // Guide
    case 1: cursorSprite_->SetPosition({ 0, 170 }); break;   // ToResult
    }

    // --- 決定 ---
    if (input->TriggerKey(DIK_SPACE) || input->TriggerKey(DIK_RETURN)) {
        if (decideSEHandle_ != 0) {
            audio_->PlayWave(decideSEHandle_, false, 1.0f);
        }
        if (menuIndex_ == 0) {
            guideActive_ = true;
        }
        else if (menuIndex_ == 1) {
            goResult_ = true;
        }
    }
}

void Pause::Draw() {
    if (!active_) return;

    pauseOverlay_->Draw();
    miniMap_->Draw();

    // ガイド中
    if (guideActive_) {
        guideSprite_->Draw();
        return;
    }

    // メニューUI
    cursorSprite_->Draw();
}
