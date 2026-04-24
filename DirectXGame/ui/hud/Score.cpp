#include "Score.h"
#include "../common/DigitSpriteUtil.h"
#include <algorithm>

using namespace KamataEngine;

namespace DirectXGame {

void Score::Initialize() {
    // 数字表示用テクスチャを読み込み
    textureHandle_ = TextureManager::Load("ui/number/numbers.png");
    position_ = { 800.0f, 10.0f };

    // 各桁分のスプライトを生成し、横並びに配置
    sprite_.clear();
    sprite_.resize(static_cast<size_t>(digitCount_));
    for (int32_t i = 0; i < digitCount_; ++i) {
        sprite_[static_cast<size_t>(i)] = std::unique_ptr<Sprite>(Sprite::Create(textureHandle_, { 0.0f, 0.0f }));
        sprite_[static_cast<size_t>(i)]->SetSize(digitSize_);
        sprite_[static_cast<size_t>(i)]->SetTextureRect({ 0.0f, 0.0f }, digitSize_);
    }

    UpdateBounds();
    RefreshLayout();
}

void Score::Update() {
    // スコアは外部から更新されるため、ここでは特別な処理は不要
}

void Score::Draw() const {
    if (!visible_) {
        return;
    }

    // 各桁スプライトを順に描画
    for (const auto& sprite : sprite_) {
        if (sprite) {
            sprite->Draw();
        }
    }
}

void Score::SetNumber(int32_t number) {
    // 数値を桁ごとに分解してスプライトに反映
    int32_t safeNumber = (std::max)(0, number);
    int32_t initialDigit = 1;
    for (int32_t i = 1; i < digitCount_; ++i) {
        initialDigit *= 10;
    }
    DigitSpriteUtil::SetNumberSprites(sprite_, digitSize_.x, digitSize_, safeNumber, initialDigit);
}

void Score::SetPosition(const Vector2& pos) {
    UIElement::SetPosition(pos);
}

void Score::SetScale(float scale) {
    // スケール値を更新し、各桁スプライトのサイズと位置を再設定
    scale_ = scale;
    UpdateBounds();
    RefreshLayout();
}

void Score::OnTransformChanged() {
    RefreshLayout();
}

void Score::SetDigitCount(int32_t digitCount) {
    digitCount_ = (std::max)(1, digitCount);
}

void Score::RefreshLayout() {
    const Vector2 world = GetWorldPosition();
    const Vector2 offset = GetAnchorOffset();
    const Vector2 basePosition = { world.x + offset.x, world.y + offset.y };

    for (int32_t i = 0; i < digitCount_; ++i) {
        if (!sprite_[static_cast<size_t>(i)]) {
            continue;
        }
        DigitSpriteUtil::UpdateDigitLayout(*sprite_[static_cast<size_t>(i)], basePosition, digitSize_, scale_, i);
    }
}

void Score::UpdateBounds() {
    size_ = { digitSize_.x * scale_ * static_cast<float>(digitCount_), digitSize_.y * scale_ };
}

} // namespace DirectXGame
