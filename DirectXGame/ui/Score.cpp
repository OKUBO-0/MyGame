#include "Score.h"

using namespace KamataEngine;

Score::Score() {}

Score::~Score() {
    // 各桁スプライトのメモリ解放
    for (int i = 0; i < kDigitCount; ++i) {
        delete sprite_[i];
    }
}

void Score::Initialize() {
    // 数字テクスチャを読み込む
    textureHandle_ = TextureManager::Load("number.png");

    // 各桁分のスプライトを生成し、横に配置
    for (int i = 0; i < kDigitCount; ++i) {
        sprite_[i] = Sprite::Create(textureHandle_, { basePosition_.x + size_.x * i, basePosition_.y });
        sprite_[i]->SetSize(size_);
        sprite_[i]->SetTextureRect({ 0.0f, 0.0f }, size_);
    }
}

void Score::Update() {
    // 現時点では特に更新処理はなし
}

void Score::Draw() {
    // スプライト描画開始
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    Sprite::PreDraw(dxCommon->GetCommandList());

    // 各桁を順に描画
    for (int i = 0; i < kDigitCount; ++i) {
        sprite_[i]->Draw();
    }

    // スプライト描画終了
    Sprite::PostDraw();
}

void Score::SetNumber(int number) {
    // 数値を各桁に分解して対応するテクスチャ領域を設定
    int32_t digit = 10000; // 5桁対応（10000の位から）

    for (int i = 0; i < kDigitCount; ++i) {
        int nowNumber = number / digit;
        sprite_[i]->SetTextureRect({ size_.x * nowNumber, 0.0f }, size_);
        number %= digit;
        digit /= 10;
    }
}

void Score::SetPosition(const Vector2& pos) {
    // スコアの基準位置を更新
    basePosition_ = pos;

    // 各桁スプライトの座標を再計算
    for (int i = 0; i < kDigitCount; ++i) {
        sprite_[i]->SetPosition({ basePosition_.x + (size_.x * scale_ * i), basePosition_.y });
    }
}

void Score::SetScale(float scale) {
    // スケール値を更新
    scale_ = scale;

    // 各桁のスプライトサイズと位置を再設定
    for (int i = 0; i < kDigitCount; ++i) {
        sprite_[i]->SetSize({ size_.x * scale_, size_.y * scale_ });
        sprite_[i]->SetPosition({ basePosition_.x + (size_.x * scale_ * i), basePosition_.y });
    }
}