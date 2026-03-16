#include "Fade.h"
#include "FadeStateIn.h"
#include "FadeStateOut.h"
#include "FadeStateStay.h"
#include "FadeStateNone.h"

using namespace KamataEngine;

const float Fade::kDefaultFadeSpeed = 0.02f;

void Fade::Initialize() {
    uint32_t blackTex = TextureManager::Load(kBlackTexturePath);

    fadeSprite_.reset(Sprite::Create(blackTex, { 0, 0 }));
    fadeSprite_->SetSize(Vector2(kScreenWidth, kScreenHeight));

    alpha_ = 1.0f;
    speed_ = kDefaultFadeSpeed;
    finished_ = false;

    ChangeState(new FadeStateIn());
}

void Fade::StartFadeIn(float speed) {
    speed_ = speed;
    alpha_ = 1.0f;
    finished_ = false;
    ChangeState(new FadeStateIn());
}

void Fade::StartFadeOut(float speed) {
    speed_ = speed;
    alpha_ = 0.0f;
    finished_ = false;
    ChangeState(new FadeStateOut());
}

void Fade::ChangeState(FadeState* newState) {
    if (state_) state_->Exit(this);
    state_.reset(newState);
    if (state_) state_->Enter(this);
}

void Fade::Update() {
    if (state_) state_->Update(this);

    if (fadeSprite_) {
        fadeSprite_->SetColor(Vector4(1, 1, 1, alpha_));
    }
}

void Fade::Draw() {
    if (!fadeSprite_) return;

    Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
    fadeSprite_->Draw();
    Sprite::PostDraw();
}