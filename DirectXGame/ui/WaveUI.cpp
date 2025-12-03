#include "WaveUI.h"
using namespace KamataEngine;

WaveUI::WaveUI() {}

WaveUI::~WaveUI() {
    /// <summary>
    /// 動的に生成したスプライトを解放
    /// </summary>
    delete waveLabel_;
    for (auto& d : digits_) { delete d; }
}

void WaveUI::Initialize() {
    /// <summary>
    /// テクスチャ読み込み（ラベル用と数字用）
    /// </summary>
    labelTexHandle_ = TextureManager::Load("wave.png");
    numberTexHandle_ = TextureManager::Load("number.png");

    /// <summary>
    /// ラベルスプライト生成（画面上部中央に「WAVE」表示）
    /// </summary>
    static constexpr float kLabelPosX = 580.0f;
    static constexpr float kLabelPosY = 50.0f;
    static constexpr float kLabelWidth = 64.0f;
    static constexpr float kLabelHeight = 32.0f;

    waveLabel_ = Sprite::Create(labelTexHandle_, { kLabelPosX, kLabelPosY });
    waveLabel_->SetSize({ kLabelWidth, kLabelHeight });

    /// <summary>
    /// 数字スプライト生成（最大2桁分を用意）
    /// </summary>
    static constexpr float kDigitStartX = 660.0f;
    for (int32_t i = 0; i < kMaxDigits; ++i) {
        digits_[i] = Sprite::Create(numberTexHandle_, { kDigitStartX + digitSize_.x * i, kLabelPosY });
        digits_[i]->SetSize(digitSize_);
        digits_[i]->SetTextureRect({ 0, 0 }, digitSize_);
    }
}

void WaveUI::SetWave(int32_t wave) {
    /// <summary>
    /// 表示する桁数を判定（1桁か2桁か）
    /// </summary>
    int32_t digitsToShow = (wave < 10) ? 1 : 2;

    /// <summary>
    /// 桁ごとに数値を分解してスプライトに反映
    /// </summary>
    int32_t digit = (digitsToShow == 1) ? 1 : 10;
    for (int32_t i = 0; i < kMaxDigits; ++i) {
        if (i < digitsToShow) {
            int32_t num = wave / digit;
            digits_[i]->SetTextureRect({ digitSize_.x * num, 0 }, digitSize_);
            digits_[i]->SetColor({ 1, 1, 1, 1 });
            wave %= digit;
            digit /= 10;
        }
        else {
            digits_[i]->SetColor({ 1, 1, 1, 0 });
        }
    }
}

void WaveUI::Update() {
    /// <summary>
    /// 必要に応じてアニメーションや点滅処理を追加可能
    /// </summary>
}

void WaveUI::Draw() {
    DirectXCommon* dx = DirectXCommon::GetInstance();
    Sprite::PreDraw(dx->GetCommandList());

    /// <summary>
    /// ラベルと数字を描画
    /// </summary>
    if (waveLabel_) { waveLabel_->Draw(); }
    for (auto& d : digits_) {
        if (d) { d->Draw(); }
    }

    Sprite::PostDraw();
}