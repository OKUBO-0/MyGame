#include "Score.h"
#include "../common/DigitSpriteUtil.h"

using namespace KamataEngine;

namespace DirectXGame {

void Score::Initialize() {
    // 数字表示用テクスチャを読み込み
    textureHandle_ = TextureManager::Load("ui/number/numbers.png");

    // 各桁分のスプライトを生成し、横並びに配置
    for (int32_t i = 0; i < kDigitCount; ++i) {
        sprite_[i] = std::unique_ptr<Sprite>(
            Sprite::Create(textureHandle_, { basePosition_.x + size_.x * i, basePosition_.y })
        );
        sprite_[i]->SetSize(size_);
        sprite_[i]->SetTextureRect({ 0.0f, 0.0f }, size_);
    }
}

void Score::Update() {
    // スコアは外部から更新されるため、ここでは特別な処理は不要
}

void Score::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    Sprite::PreDraw(dxCommon->GetCommandList());

    // 各桁スプライトを順に描画
    for (int32_t i = 0; i < kDigitCount; ++i) {
        if (sprite_[i]) { sprite_[i]->Draw(); }
    }

    Sprite::PostDraw();
}

void Score::SetNumber(int32_t number) {
    // 数値を桁ごとに分解してスプライトに反映
    static constexpr int32_t kInitialDigit = 10000; // 5桁対応（10000の位から処理）
    DigitSpriteUtil::SetNumberSprites(sprite_, size_.x, size_, number, kInitialDigit);
}

void Score::SetPosition(const Vector2& pos) {
    // 基準位置を更新し、各桁スプライトの座標を再計算
    basePosition_ = pos;

    for (int32_t i = 0; i < kDigitCount; ++i) {
        sprite_[i]->SetPosition(DigitSpriteUtil::CalculateDigitPosition(basePosition_, size_, scale_, i));
    }
}

void Score::SetScale(float scale) {
    // スケール値を更新し、各桁スプライトのサイズと位置を再設定
    scale_ = scale;

    for (int32_t i = 0; i < kDigitCount; ++i) {
        DigitSpriteUtil::UpdateDigitLayout(*sprite_[i], basePosition_, size_, scale_, i);
    }
}

} // namespace DirectXGame
