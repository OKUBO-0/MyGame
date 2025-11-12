#include "Fade.h"

using namespace KamataEngine;

Fade::Fade() {
}

Fade::~Fade() {
	delete fadeSprite_;
}

void Fade::Initialize() {
	uint32_t blackTex = TextureManager::Load("color/black.png");
	fadeSprite_ = Sprite::Create(blackTex, { 0, 0 });
	fadeSprite_->SetSize(Vector2(1280, 720));
	alpha_ = 1.0f;
	speed_ = 0.02f;
	state_ = State::FadeIn;
	finished_ = false;
}

void Fade::StartFadeIn(float speed) {
	state_ = State::FadeIn;
	speed_ = speed;
	alpha_ = 1.0f;
	finished_ = false;
}

void Fade::StartFadeOut(float speed) {
	state_ = State::FadeOut;
	speed_ = speed;
	alpha_ = 0.0f;
	finished_ = false;
}

void Fade::Update() {
	switch (state_) {
	case State::FadeIn:
		alpha_ -= speed_;
		if (alpha_ <= 0.0f) {
			alpha_ = 0.0f;
			state_ = State::Stay;
			finished_ = true;
		}
		break;

	case State::FadeOut:
		alpha_ += speed_;
		if (alpha_ >= 1.0f) {
			alpha_ = 1.0f;
			state_ = State::None;
			finished_ = true;
		}
		break;

	default:
		break;
	}

	fadeSprite_->SetColor(Vector4(1, 1, 1, alpha_));
}

void Fade::Draw() {
	if (state_ != State::None) {
		Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
		fadeSprite_->Draw();
		Sprite::PostDraw();
	}
}