#include "HpGauge.h"
using namespace KamataEngine;

HpGauge::HpGauge() {}
HpGauge::~HpGauge() {
    delete blackGauge_;
    delete redGauge_;
}

void HpGauge::Initialize() {
    dummyTextureHandle_ = TextureManager::Load("white1x1.png");

    // 黒背景（ゲージの土台）
    blackGauge_ = Sprite::Create(dummyTextureHandle_, { 600, 450 });
    blackGauge_->SetSize({ 80, 10 });
    blackGauge_->SetColor({ 0.0f, 0.0f, 0.0f, 0.85f });

    // 緑ゲージ（現在のHP）
    redGauge_ = Sprite::Create(dummyTextureHandle_, { 600, 450 });
    redGauge_->SetSize({ 0, 10 });
    redGauge_->SetColor({ 1.0f, 0.0f, 0.0f, 0.95f });
}

void HpGauge::SetHP(int current, int max) {
    targetHP_ = current;
    maxHP_ = max(1, max);
}

void HpGauge::Update() {
    if (displayedHP_ < targetHP_) {
        displayedHP_ += max(1, (targetHP_ - displayedHP_) / 10);
    }
    else if (displayedHP_ > targetHP_) {
        displayedHP_ -= max(1, (displayedHP_ - targetHP_) / 10);
    }

    float ratio = static_cast<float>(displayedHP_) / static_cast<float>(maxHP_);
    ratio = std::clamp(ratio, 0.0f, 1.0f);
    float maxWidth = 80.0f;
    float width = maxWidth * ratio;
    redGauge_->SetSize({ width, 10 });
}

void HpGauge::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    Sprite::PreDraw(dxCommon->GetCommandList());

    if (blackGauge_) blackGauge_->Draw();
    if (redGauge_) redGauge_->Draw();

    Sprite::PostDraw();
}

// HealthGauge.cpp
bool HpGauge::IsDepleted() const {
    return displayedHP_ <= 0;
}