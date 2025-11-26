#include "WaveUI.h"
using namespace KamataEngine;

WaveUI::WaveUI() {}
WaveUI::~WaveUI() {
    // 動的に生成したスプライトを解放
    delete waveLabel_;
    for (auto& d : digits_) delete d;
}

void WaveUI::Initialize() {
    // テクスチャ読み込み（ラベル用と数字用）
    labelTexHandle_ = TextureManager::Load("wave.png");
    numberTexHandle_ = TextureManager::Load("number.png");

    // ラベルスプライト生成（画面上部中央に「WAVE」表示）
    waveLabel_ = Sprite::Create(labelTexHandle_, { 580.0f, 50.0f });
    waveLabel_->SetSize({ 64, 32 });

    // 数字スプライト生成（最大2桁分を用意）
    for (int i = 0; i < kMaxDigits; ++i) {
        digits_[i] = Sprite::Create(numberTexHandle_, { 660.0f + digitSize_.x * i, 50.0f });
        digits_[i]->SetSize(digitSize_);
        // 初期状態は「0」を表示
        digits_[i]->SetTextureRect({ 0, 0 }, digitSize_);
    }
}

void WaveUI::SetWave(int wave) {
    // 表示する桁数を判定（1桁か2桁か）
    int digitsToShow = (wave < 10) ? 1 : 2;

    // 桁ごとに数値を分解してスプライトに反映
    int digit = (digitsToShow == 1) ? 1 : 10;
    for (int i = 0; i < kMaxDigits; ++i) {
        if (i < digitsToShow) {
            int num = wave / digit; // 現在の桁の数値
            digits_[i]->SetTextureRect({ digitSize_.x * num, 0 }, digitSize_);
            digits_[i]->SetColor({ 1, 1, 1, 1 }); // 表示
            wave %= digit;
            digit /= 10;
        }
        else {
            // 不要な桁は非表示（透明化）
            digits_[i]->SetColor({ 1, 1, 1, 0 });
        }
    }
}

void WaveUI::Update() {
    // 必要に応じてアニメーションや点滅処理を追加可能
}

void WaveUI::Draw() {
    DirectXCommon* dx = DirectXCommon::GetInstance();
    Sprite::PreDraw(dx->GetCommandList());

    // ラベルと数字を描画
    waveLabel_->Draw();
    for (auto& d : digits_) d->Draw();

    Sprite::PostDraw();
}