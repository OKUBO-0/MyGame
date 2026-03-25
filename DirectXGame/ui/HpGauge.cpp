#include "HpGauge.h"
using namespace KamataEngine;

namespace {

int32_t StepDisplayValue(int32_t displayedValue, int32_t targetValue) {
    if (displayedValue < targetValue) {
        displayedValue += std::max<int32_t>(1, (targetValue - displayedValue) / 10);
    } else if (displayedValue > targetValue) {
        displayedValue -= std::max<int32_t>(1, (displayedValue - targetValue) / 10);
    }

    return displayedValue;
}

float CalculateGaugeWidth(int32_t displayedValue, int32_t maxValue, float maxGaugeWidth) {
    float ratio = static_cast<float>(displayedValue) / static_cast<float>(maxValue);
    ratio = std::clamp(ratio, 0.0f, 1.0f);
    return maxGaugeWidth * ratio;
}

} // namespace

void HpGauge::Initialize() {
    // 白1x1テクスチャを読み込み（色付き矩形として利用）
    dummyTextureHandle_ = TextureManager::Load("textures/debug/white1x1.png");

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
    static constexpr float kMaxGaugeWidth = 80.0f; // ゲージ最大幅
    static constexpr float kGaugeHeight = 10.0f;   // ゲージ高さ

    // 表示HPを滑らかに変化させる（目標値に徐々に近づける）
    displayedHP_ = StepDisplayValue(displayedHP_, targetHP_);

    // HP比率を計算し、赤ゲージの幅に反映
    float width = CalculateGaugeWidth(displayedHP_, maxHP_, kMaxGaugeWidth);
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
