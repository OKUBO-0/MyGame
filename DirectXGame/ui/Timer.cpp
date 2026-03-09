#include "Timer.h"
using namespace KamataEngine;

void Timer::Initialize() {

    numberTexture_ = TextureManager::Load("number/numbers.png");
    colonTexture_ = TextureManager::Load("number/colon.png");

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
    sprite_[0]->SetTextureRect({ size_.x * m1, 0.0f }, size_);
    sprite_[1]->SetTextureRect({ size_.x * m2, 0.0f }, size_);
    sprite_[3]->SetTextureRect({ size_.x * s1, 0.0f }, size_);
    sprite_[4]->SetTextureRect({ size_.x * s2, 0.0f }, size_);
}

void Timer::SetPosition(const Vector2& pos) {
    basePosition_ = pos;

    for (int i = 0; i < kDigitCount; i++) {
        sprite_[i]->SetPosition({ basePosition_.x + (size_.x * scale_ * i), basePosition_.y });
    }

    colonSprite_->SetPosition({ basePosition_.x + (size_.x * scale_ * 2), basePosition_.y });
}

void Timer::SetScale(float scale) {
    scale_ = scale;

    for (int i = 0; i < kDigitCount; i++) {
        sprite_[i]->SetSize({ size_.x * scale_, size_.y * scale_ });
        sprite_[i]->SetPosition({ basePosition_.x + (size_.x * scale_ * i), basePosition_.y });
    }

    colonSprite_->SetSize({ size_.x * scale_, size_.y * scale_ });
    colonSprite_->SetPosition({ basePosition_.x + (size_.x * scale_ * 2), basePosition_.y });
}