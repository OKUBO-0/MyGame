#include "Timer.h"
#include "../common/DigitSpriteUtil.h"
#include <algorithm>
using namespace KamataEngine;

namespace DirectXGame {

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
    // M1 M2 S1 S2
    for (int i = 0; i < kDigitCount; i++) {
        if (i == 2) continue; // 2番目はコロンの位置
        sprite_[i]->Draw();
    }

    // コロン
    colonSprite_->Draw();
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
    DigitSpriteUtil::SetDigitSprite(*sprite_[0], size_.x, size_, m1);
    DigitSpriteUtil::SetDigitSprite(*sprite_[1], size_.x, size_, m2);
    DigitSpriteUtil::SetDigitSprite(*sprite_[3], size_.x, size_, s1);
    DigitSpriteUtil::SetDigitSprite(*sprite_[4], size_.x, size_, s2);
}

void Timer::SetPosition(const Vector2& pos) {
    basePosition_ = pos;

    for (int i = 0; i < kDigitCount; i++) {
        sprite_[i]->SetPosition(DigitSpriteUtil::CalculateDigitPosition(basePosition_, size_, scale_, i));
    }

    colonSprite_->SetPosition(DigitSpriteUtil::CalculateDigitPosition(basePosition_, size_, scale_, 2));
}

void Timer::SetScale(float scale) {
    scale_ = scale;

    for (int i = 0; i < kDigitCount; i++) {
        DigitSpriteUtil::UpdateDigitLayout(*sprite_[i], basePosition_, size_, scale_, i);
    }

    DigitSpriteUtil::UpdateDigitLayout(*colonSprite_, basePosition_, size_, scale_, 2);
}

void Timer::SetTime(float time) {
    time_ = (std::max)(0.0f, time);
    UpdateDisplay();
}

} // namespace DirectXGame
