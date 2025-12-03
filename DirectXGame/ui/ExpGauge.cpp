#include "ExpGauge.h"
#include <algorithm>
using namespace KamataEngine;

ExpGauge::ExpGauge() {}

ExpGauge::~ExpGauge() {
    /// <summary>
    /// 動的に生成したスプライトを解放
    /// </summary>
    delete yellowFrame_;
    delete blackGauge_;
    delete blueGauge_;
    delete lvLabel_;
    for (int32_t i = 0; i < kLvDigits; ++i) {
        delete sprite_[i];
    }
}

void ExpGauge::Initialize() {
    /// <summary>
    /// テクスチャ読み込み（白1x1は色付き矩形用）
    /// </summary>
    dummyTextureHandle_ = TextureManager::Load("white1x1.png");
    lvLabelHandle_ = TextureManager::Load("lv_label.png");
    lvDigitsHandle_ = TextureManager::Load("number.png");

    /// <summary>
    /// 外枠（黄色のフレーム）
    /// </summary>
    yellowFrame_ = Sprite::Create(dummyTextureHandle_, { 0, 1 });
    yellowFrame_->SetSize({ 1280, 50 });
    yellowFrame_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });

    /// <summary>
    /// 黒背景（ゲージの土台部分）
    /// </summary>
    blackGauge_ = Sprite::Create(dummyTextureHandle_, { 5, 6 });
    blackGauge_->SetSize({ 1270, 40 });
    blackGauge_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

    /// <summary>
    /// 青ゲージ（現在のEXPを表す部分）
    /// </summary>
    blueGauge_ = Sprite::Create(dummyTextureHandle_, { 5, 6 });
    blueGauge_->SetSize({ 0, 40 }); // 初期値は0幅
    blueGauge_->SetColor({ 0.0f, 0.0f, 1.0f, 1.0f });

    /// <summary>
    /// LVラベル（「LV」文字画像）
    /// </summary>
    lvLabel_ = Sprite::Create(lvLabelHandle_, { 1200.0f, 10.0f });
    lvLabel_->SetSize({ 32, 32 });

    /// <summary>
    /// レベル数字（2桁分を用意）
    /// </summary>
    for (int32_t i = 0; i < kLvDigits; ++i) {
        sprite_[i] = Sprite::Create(lvDigitsHandle_, { 1235.0f + size_.x * i, 10.0f });
        sprite_[i]->SetSize(size_);
        sprite_[i]->SetTextureRect({ 0.0f, 0.0f }, size_);
    }
}

void ExpGauge::SetEXP(int32_t current, int32_t max) {
    /// <summary>
    /// 現在EXPと最大EXPを設定（maxは最低1にして0除算防止）
    /// </summary>
    targetExp_ = current;
    maxExp_ = std::max<int32_t>(1, max); // 修正: テンプレート引数を明示
}

void ExpGauge::SetLevel(int32_t level) {
    /// <summary>
    /// レベル数値を桁ごとに分解してスプライトに反映
    /// </summary>
    int32_t digit = 10; // 10の位から処理
    for (int32_t i = 0; i < kLvDigits; ++i) {
        int32_t nowNumber = level / digit;
        sprite_[i]->SetTextureRect({ size_.x * nowNumber, 0.0f }, size_);
        level %= digit;
        digit /= 10;
    }
}

void ExpGauge::Update() {
    /// <summary>
    /// EXP表示を滑らかに変化させる（目標値に徐々に近づける）
    /// </summary>
    if (displayedExp_ < targetExp_) {
        displayedExp_ += std::max<int32_t>(1, (targetExp_ - displayedExp_) / 10);
    }
    else if (displayedExp_ > targetExp_) {
        displayedExp_ -= std::max<int32_t>(1, (displayedExp_ - targetExp_) / 10);
    }

    /// <summary>
    /// EXP比率を計算してゲージ幅に反映
    /// </summary>
    float ratio = static_cast<float>(displayedExp_) / static_cast<float>(maxExp_);
    ratio = std::clamp(ratio, 0.0f, 1.0f);

    static constexpr float kMaxGaugeWidth = 1280.0f; ///< ゲージ最大幅
    static constexpr float kGaugeHeight = 40.0f;     ///< ゲージ高さ

    float width = kMaxGaugeWidth * ratio;
    blueGauge_->SetSize({ width, kGaugeHeight });
}

void ExpGauge::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    Sprite::PreDraw(dxCommon->GetCommandList());

    /// <summary>
    /// 各スプライトを描画（順序は背景→ゲージ→ラベル→数字）
    /// </summary>
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
    /// <summary>
    /// 現在EXPが最大値に到達しているか判定
    /// </summary>
    return displayedExp_ >= maxExp_;
}