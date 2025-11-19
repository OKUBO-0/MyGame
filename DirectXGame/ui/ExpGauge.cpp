#include "ExpGauge.h"
using namespace KamataEngine;

ExpGauge::ExpGauge() {}
ExpGauge::~ExpGauge() {
    delete yellowFrame_;
    delete blackGauge_;
    delete blueGauge_;
    delete lvLabel_;
    for (int i = 0; i < lvDigits_; ++i) {
        delete sprite_[i];
    }
}

void ExpGauge::Initialize() {
    dummyTextureHandle_ = TextureManager::Load("white1x1.png");
    lvLabelHandle_ = TextureManager::Load("lv_label.png");
    lvDigitsHandle_ = TextureManager::Load("number.png");

    // 黄色のふち（外枠）
    yellowFrame_ = Sprite::Create(dummyTextureHandle_, { 0, 1 });
    yellowFrame_->SetSize({ 1280, 50 });
    yellowFrame_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });

    // 黒背景（ゲージの土台）
    blackGauge_ = Sprite::Create(dummyTextureHandle_, { 5, 6 });
    blackGauge_->SetSize({ 1270, 40 });
    blackGauge_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

    // 青ゲージ（現在のEXP）
    blueGauge_ = Sprite::Create(dummyTextureHandle_, { 5, 6 });
    blueGauge_->SetSize({ 0, 40 });
    blueGauge_->SetColor({ 0.0f, 0.0f, 1.0f, 1.0f });

    // [LV] ラベル
    lvLabel_ = Sprite::Create(lvLabelHandle_, { 1200.0f, 10.0f });
    lvLabel_->SetSize({ 32, 32 });

    // レベル数字（2桁）
    for (int i = 0; i < lvDigits_; ++i) {
        sprite_[i] = Sprite::Create(lvDigitsHandle_, { 1235.0f + size_.x * i, 10.0f });
        sprite_[i]->SetSize(size_);
        sprite_[i]->SetTextureRect({ 0.0f, 0.0f }, size_);
    }
}

void ExpGauge::SetEXP(int current, int max) {
    targetExp_ = current;
    maxExp_ = max(1, max); // 0除算防止
}

void ExpGauge::SetLevel(int level) {
    int digit = 10; // 10の位から
    for (int i = 0; i < lvDigits_; ++i) {
        int nowNumber = level / digit;
        sprite_[i]->SetTextureRect({ size_.x * nowNumber, 0.0f }, size_);
        level %= digit;
        digit /= 10;
    }
}

void ExpGauge::Update() {
    if (displayedExp_ < targetExp_) {
        displayedExp_ += max(1, (targetExp_ - displayedExp_) / 10);
    }
    else if (displayedExp_ > targetExp_) {
        displayedExp_ -= max(1, (displayedExp_ - targetExp_) / 10);
    }

    float ratio = static_cast<float>(displayedExp_) / static_cast<float>(maxExp_);
    ratio = std::clamp(ratio, 0.0f, 1.0f);
    float maxWidth = 1280.0f;
    float width = maxWidth * ratio;
    blueGauge_->SetSize({ width, 40 });
}

void ExpGauge::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    Sprite::PreDraw(dxCommon->GetCommandList());

    if (yellowFrame_) yellowFrame_->Draw();
    if (blackGauge_) blackGauge_->Draw();
    if (blueGauge_) blueGauge_->Draw();
    if (lvLabel_) lvLabel_->Draw();
    for (int i = 0; i < lvDigits_; ++i) {
        if (sprite_[i]) sprite_[i]->Draw();
    }

    Sprite::PostDraw();
}

// ExpGauge.cpp
bool ExpGauge::IsFilled() const {
    return displayedExp_ >= maxExp_;
}