#include "CurtainTransition.h"
using namespace KamataEngine;

void CurtainTransition::Initialize() {
    uint32_t tex = TextureManager::Load("textures/color/black.png");

    topCurtain_.reset(Sprite::Create(tex, { 0, -720 }));
    bottomCurtain_.reset(Sprite::Create(tex, { 0, 720 }));

    topCurtain_->SetSize({ 1280, 720 });
    bottomCurtain_->SetSize({ 1280, 720 });

    state_ = State::kNone;
}

void CurtainTransition::StartClose(float speed) {
    speed_ = speed;
    state_ = State::kClose;

    topCurtain_->SetPosition({ 0, -720 });
    bottomCurtain_->SetPosition({ 0, 720 });
}

void CurtainTransition::StartOpen(float speed) {
    speed_ = speed;
    state_ = State::kOpen;

    topCurtain_->SetPosition({ 0, 0 });
    bottomCurtain_->SetPosition({ 0, 0 });
}

void CurtainTransition::Update() {
    if (state_ == State::kClose) {
        auto posTop = topCurtain_->GetPosition();
        auto posBottom = bottomCurtain_->GetPosition();

        posTop.y += speed_;
        posBottom.y -= speed_;

        topCurtain_->SetPosition(posTop);
        bottomCurtain_->SetPosition(posBottom);

        if (posTop.y >= 0) {
            topCurtain_->SetPosition({ 0, 0 });
            bottomCurtain_->SetPosition({ 0, 0 });
            state_ = State::kFinished;
        }
    }
    else if (state_ == State::kOpen) {
        auto posTop = topCurtain_->GetPosition();
        auto posBottom = bottomCurtain_->GetPosition();

        posTop.y -= speed_;
        posBottom.y += speed_;

        topCurtain_->SetPosition(posTop);
        bottomCurtain_->SetPosition(posBottom);

        if (posTop.y <= -720) {
            state_ = State::kNone;
        }
    }
}

void CurtainTransition::Draw() {
    if (state_ == State::kNone) return;

    Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
    topCurtain_->Draw();
    bottomCurtain_->Draw();
    Sprite::PostDraw();
}
