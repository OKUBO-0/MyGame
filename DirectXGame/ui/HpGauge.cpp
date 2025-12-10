#include "HpGauge.h"
using namespace KamataEngine;

void HpGauge::Initialize() {
    // 白1x1テクスチャを読み込み（色付き矩形として利用）
    dummyTextureHandle_ = TextureManager::Load("white1x1.png");

    // HPゲージの背景（黒色の土台）
    blackGauge_ = std::unique_ptr<Sprite>(Sprite::Create(dummyTextureHandle_, { 600, 450 }));
    blackGauge_->SetSize({ 80, 10 });
    blackGauge_->SetColor({ 0.0f, 0.0f, 0.0f, 0.85f });

    // HPゲージ本体（赤色で残りHPを表現）
    redGauge_ = std::unique_ptr<Sprite>(Sprite::Create(dummyTextureHandle_, { 600, 450 }));
    redGauge_->SetSize({ 0, 10 });
    redGauge_->SetColor({ 1.0f, 0.0f, 0.0f, 0.95f });
}

void HpGauge::SetHP(int32_t current, int32_t max) {
    // 現在HPと最大HPを設定（maxは最低1にして0除算防止）
    targetHP_ = current;
    maxHP_ = std::max<int32_t>(1, max);
}

void HpGauge::Update() {
    // 表示HPを滑らかに変化させる（目標値に徐々に近づける）
    if (displayedHP_ < targetHP_) {
        displayedHP_ += std::max<int32_t>(1, (targetHP_ - displayedHP_) / 10);
    }
    else if (displayedHP_ > targetHP_) {
        displayedHP_ -= std::max<int32_t>(1, (displayedHP_ - targetHP_) / 10);
    }

    // HP比率を計算し、赤ゲージの幅に反映
    float ratio = static_cast<float>(displayedHP_) / static_cast<float>(maxHP_);
    ratio = std::clamp(ratio, 0.0f, 1.0f);

    static constexpr float kMaxGaugeWidth = 80.0f; // ゲージ最大幅
    static constexpr float kGaugeHeight = 10.0f;   // ゲージ高さ

    float width = kMaxGaugeWidth * ratio;
    redGauge_->SetSize({ width, kGaugeHeight });
}

void HpGauge::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    Sprite::PreDraw(dxCommon->GetCommandList());

    // 背景 → 赤ゲージの順で描画
    if (blackGauge_) { blackGauge_->Draw(); }
    if (redGauge_) { redGauge_->Draw(); }

    Sprite::PostDraw();
}

bool HpGauge::IsDepleted() const {
    // HPが0以下になったかどうかを判定
    return displayedHP_ <= 0;
}