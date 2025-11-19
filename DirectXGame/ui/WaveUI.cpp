#include "WaveUI.h"
using namespace KamataEngine;

WaveUI::WaveUI() {}
WaveUI::~WaveUI() {
    delete waveLabel_;
    for (auto& d : digits_) delete d;
}

void WaveUI::Initialize() {
    labelTexHandle_ = TextureManager::Load("wave.png");
    numberTexHandle_ = TextureManager::Load("number.png");

    // ラベル：[WAVE]
    waveLabel_ = Sprite::Create(labelTexHandle_, { 580.0f, 50.0f }); // 中央上
    waveLabel_->SetSize({ 64, 32 });

    // 数字（2桁）
    for (int i = 0; i < kMaxDigits; ++i) {
        digits_[i] = Sprite::Create(numberTexHandle_, { 660.0f + digitSize_.x * i, 50.0f });
        digits_[i]->SetSize(digitSize_);
        digits_[i]->SetTextureRect({ 0, 0 }, digitSize_);
    }
}

void WaveUI::SetWave(int wave) {
    // 1桁か2桁か判定
    int digitsToShow = (wave < 10) ? 1 : 2;

    int digit = (digitsToShow == 1) ? 1 : 10;
    for (int i = 0; i < kMaxDigits; ++i) {
        if (i < digitsToShow) {
            int num = wave / digit;
            digits_[i]->SetTextureRect({ digitSize_.x * num, 0 }, digitSize_);
            digits_[i]->SetColor({ 1, 1, 1, 1 }); // 表示
            wave %= digit;
            digit /= 10;
        }
        else {
            digits_[i]->SetColor({ 1, 1, 1, 0 }); // 非表示（透明）
        }
    }
}

void WaveUI::Update() {
    // 必要に応じてアニメーションや点滅処理など
}

void WaveUI::Draw() {
    DirectXCommon* dx = DirectXCommon::GetInstance();
    Sprite::PreDraw(dx->GetCommandList());

    waveLabel_->Draw();
    for (auto& d : digits_) d->Draw();

    Sprite::PostDraw();
}