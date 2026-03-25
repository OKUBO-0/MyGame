#include "Timer.h"
using namespace KamataEngine;

namespace {

Vector2 CalculateDigitPosition(const Vector2& basePosition, const Vector2& size, float scale, int32_t index) {
    return { basePosition.x + (size.x * scale * index), basePosition.y };
}

void UpdateDigitLayout(Sprite& sprite, const Vector2& basePosition, const Vector2& size, float scale, int32_t index) {
    sprite.SetSize({ size.x * scale, size.y * scale });
    sprite.SetPosition(CalculateDigitPosition(basePosition, size, scale, index));
}

void SetDigitSprite(Sprite& sprite, float digitWidth, const Vector2& size, int32_t number) {
    sprite.SetTextureRect({ digitWidth * number, 0.0f }, size);
}

} // namespace

void Timer::Initialize() {

    numberTexture_ = TextureManager::Load("ui/number/numbers.png");
    colonTexture_ = TextureManager::Load("ui/number/colon.png");

    // M1 M2 : S1 S2 の 5 つを生成
    for (int i = 0; i < kDigitCount; i++) {
        sprite_[i] = std::unique_ptr<Sprite>(
            Sprite::Create(numberTexture_, { basePosition_.x + size_.x * i, basePosition_.y })
        );
        sprite_[i]->SetSize(size_);
        sprite_[i]->SetTextureRect({ 0.0f, 0.0f }, size_);
    }

    // コロンは中央に配置（digit 2 の位置）
    colonSprite_ = std::unique_ptr<Sprite>(
        Sprite::Create(colonTexture_, { basePosition_.x + size_.x * 2, basePosition_.y })
    );
    colonSprite_->SetSize(size_);
}

void Timer::Update(float dt) {
    time_ += dt;
    UpdateDisplay();
}

void Timer::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    Sprite::PreDraw(dxCommon->GetCommandList());

    // M1 M2 S1 S2
    for (int i = 0; i < kDigitCount; i++) {
        if (i == 2) continue; // 2番目はコロンの位置
        sprite_[i]->Draw();
    }

    // コロン
    colonSprite_->Draw();

    Sprite::PostDraw();
}

void Timer::UpdateDisplay() {

    int totalSec = static_cast<int>(time_);
    int minutes = totalSec / 60;
    int seconds = totalSec % 60;

    int m1 = minutes / 10;
    int m2 = minutes % 10;
    int s1 = seconds / 10;
    int s2 = seconds % 10;

    // Score と同じ：SetTextureRect({ left, top }, size_)
    SetDigitSprite(*sprite_[0], size_.x, size_, m1);
    SetDigitSprite(*sprite_[1], size_.x, size_, m2);
    SetDigitSprite(*sprite_[3], size_.x, size_, s1);
    SetDigitSprite(*sprite_[4], size_.x, size_, s2);
}

void Timer::SetPosition(const Vector2& pos) {
    basePosition_ = pos;

    for (int i = 0; i < kDigitCount; i++) {
        sprite_[i]->SetPosition(CalculateDigitPosition(basePosition_, size_, scale_, i));
    }

    colonSprite_->SetPosition(CalculateDigitPosition(basePosition_, size_, scale_, 2));
}

void Timer::SetScale(float scale) {
    scale_ = scale;

    for (int i = 0; i < kDigitCount; i++) {
        UpdateDigitLayout(*sprite_[i], basePosition_, size_, scale_, i);
    }

    UpdateDigitLayout(*colonSprite_, basePosition_, size_, scale_, 2);
}
