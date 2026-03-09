#include "ExpGauge.h"
#include <algorithm>
using namespace KamataEngine;

void ExpGauge::Initialize() {
    // テクスチャ読み込み（白1x1は色付き矩形用）
    dummyTextureHandle_ = TextureManager::Load("white1x1.png");
    lvLabelHandle_ = TextureManager::Load("lv_label.png");
    lvDigitsHandle_ = TextureManager::Load("number/numbers.png");

    // 外枠（黄色のフレーム）
    yellowFrame_ = std::unique_ptr<Sprite>(Sprite::Create(dummyTextureHandle_, { 0, 1 }));
    yellowFrame_->SetSize({ 1280, 50 });
    yellowFrame_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });

    // 黒背景（ゲージの土台部分）
    blackGauge_ = std::unique_ptr<Sprite>(Sprite::Create(dummyTextureHandle_, { 5, 6 }));
    blackGauge_->SetSize({ 1270, 40 });
    blackGauge_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

    // 青ゲージ（現在のEXPを表す部分）
    blueGauge_ = std::unique_ptr<Sprite>(Sprite::Create(dummyTextureHandle_, { 5, 6 }));
    blueGauge_->SetSize({ 0, 40 }); // 初期値は0幅
    blueGauge_->SetColor({ 0.0f, 0.0f, 1.0f, 1.0f });

    // LVラベル（「LV」文字画像）
    lvLabel_ = std::unique_ptr<Sprite>(Sprite::Create(lvLabelHandle_, { 1175.0f, 10.0f }));
    lvLabel_->SetSize({ 48, 32 });

    // レベル数字（2桁分を用意）
    for (int32_t i = 0; i < kLvDigits; ++i) {
        sprite_[i] = std::unique_ptr<Sprite>(Sprite::Create(lvDigitsHandle_, { 1225.0f + size_.x * i, 10.0f }));
        sprite_[i]->SetSize(size_);
        sprite_[i]->SetTextureRect({ 0.0f, 0.0f }, size_);
    }
}

void ExpGauge::SetEXP(int32_t current, int32_t max) {
    // 現在EXPと最大EXPを設定（maxは最低1にして0除算防止）
    targetExp_ = current;
    maxExp_ = std::max<int32_t>(1, max);
}

void ExpGauge::SetLevel(int32_t level) {
    // レベル数値を桁ごとに分解してスプライトに反映
    int32_t digit = 10; // 10の位から処理
    for (int32_t i = 0; i < kLvDigits; ++i) {
        int32_t nowNumber = level / digit;
        sprite_[i]->SetTextureRect({ size_.x * nowNumber, 0.0f }, size_);
        level %= digit;
        digit /= 10;
    }
}

void ExpGauge::Update() {
    // EXP表示を滑らかに変化させる（目標値に徐々に近づける）
    if (displayedExp_ < targetExp_) {
        displayedExp_ += std::max<int32_t>(1, (targetExp_ - displayedExp_) / 10);
    }
    else if (displayedExp_ > targetExp_) {
        displayedExp_ -= std::max<int32_t>(1, (displayedExp_ - targetExp_) / 10);
    }

    // EXP比率を計算してゲージ幅に反映
    float ratio = static_cast<float>(displayedExp_) / static_cast<float>(maxExp_);
    ratio = std::clamp(ratio, 0.0f, 1.0f);

    static constexpr float kMaxGaugeWidth = 1280.0f; // ゲージ最大幅
    static constexpr float kGaugeHeight = 40.0f;     // ゲージ高さ

    float width = kMaxGaugeWidth * ratio;
    blueGauge_->SetSize({ width, kGaugeHeight });
}

void ExpGauge::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    Sprite::PreDraw(dxCommon->GetCommandList());

    // 各スプライトを描画（順序は背景→ゲージ→ラベル→数字）
    if (yellowFrame_) { yellowFrame_->Draw(); }
    if (blackGauge_) { blackGauge_->Draw(); }
    if (blueGauge_) { blueGauge_->Draw(); }
    if (lvLabel_) { lvLabel_->Draw(); }
    for (int32_t i = 0; i < kLvDigits; ++i) {
        if (sprite_[i]) { sprite_[i]->Draw(); }
    }

    Sprite::PostDraw();
}

bool ExpGauge::IsFilled() const {
    // 現在EXPが最大値に到達しているか判定
    return displayedExp_ >= maxExp_;
}