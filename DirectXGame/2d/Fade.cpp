#include "Fade.h"

using namespace KamataEngine;

Fade::Fade() {
}

Fade::~Fade() {
    // フェード用スプライトの破棄（動的確保したリソースを解放）
    delete fadeSprite_;
}

void Fade::Initialize() {
    // 黒色テクスチャを読み込み、画面全体を覆うスプライトを生成
    uint32_t blackTex = TextureManager::Load("color/black.png");
    fadeSprite_ = Sprite::Create(blackTex, { 0, 0 });
    fadeSprite_->SetSize(Vector2(1280, 720)); // 画面サイズに合わせる

    // 初期状態は「フェードイン中」から開始
    alpha_ = 1.0f;          // 完全に黒（不透明）
    speed_ = 0.02f;         // フェード速度（1フレームごとに変化するアルファ値）
    state_ = State::FadeIn; // フェードイン状態
    finished_ = false;      // フェード完了フラグ
}

void Fade::StartFadeIn(float speed) {
    // フェードイン開始：黒から透明へ徐々に変化
    state_ = State::FadeIn;
    speed_ = speed;
    alpha_ = 1.0f;     // 最初は黒（不透明）
    finished_ = false; // 完了フラグをリセット
}

void Fade::StartFadeOut(float speed) {
    // フェードアウト開始：透明から黒へ徐々に変化
    state_ = State::FadeOut;
    speed_ = speed;
    alpha_ = 0.0f;     // 最初は透明
    finished_ = false; // 完了フラグをリセット
}

void Fade::Update() {
    // 現在の状態に応じてアルファ値を更新
    switch (state_) {
    case State::FadeIn:
        // フェードイン：黒から透明へ
        alpha_ -= speed_;
        if (alpha_ <= 0.0f) {
            alpha_ = 0.0f;       // 完全に透明
            state_ = State::Stay; // フェード終了後は「表示維持」状態
            finished_ = true;    // フェード完了
        }
        break;

    case State::FadeOut:
        // フェードアウト：透明から黒へ
        alpha_ += speed_;
        if (alpha_ >= 1.0f) {
            alpha_ = 1.0f;       // 完全に黒
            state_ = State::None; // フェード終了後は「非表示」状態
            finished_ = true;    // フェード完了
        }
        break;

    default:
        // Stay や None の場合は処理なし
        break;
    }

    // スプライトの色（アルファ値）を更新
    fadeSprite_->SetColor(Vector4(1, 1, 1, alpha_));
}

void Fade::Draw() {
    // フェードが有効な状態のみ描画
    if (state_ != State::None) {
        Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
        fadeSprite_->Draw();
        Sprite::PostDraw();
    }
}