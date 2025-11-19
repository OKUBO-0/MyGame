#include "ResultScene.h"
using namespace KamataEngine;

ResultScene::ResultScene() {}
ResultScene::~ResultScene() {
    delete backgroundSprite_;
    delete resultSprite_;
	delete resultUI_;
    delete scoreUI_;
}

void ResultScene::Initialize() {
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    uint32_t blackTex = TextureManager::Load("color/black.png");
    backgroundSprite_ = Sprite::Create(blackTex, { 0,0 });
    backgroundSprite_->SetSize({ 1280,720 });

    uint32_t resultTex = TextureManager::Load("result/result.png");
    resultSprite_ = Sprite::Create(resultTex, { 0,0 });
    resultSprite_->SetSize({ 1280,720 });

    uint32_t uiTex = TextureManager::Load("result/finish_ui.png");
	resultUI_ = Sprite::Create(uiTex, { 0,0 });
	resultUI_->SetSize({ 1280,720 });

    // ✅ Score初期化
    scoreUI_ = new Score();
    scoreUI_->Initialize();
    scoreUI_->SetNumber(0); // 最初は0
    scoreUI_->SetPosition({ 600, 400 }); // 中央付近
    scoreUI_->SetScale(2.0f);

    currentScore_ = 0;
    targetScore_ = GameData::totalEXP;

    fade_.Initialize();
    fadeOutStarted_ = false;
}

void ResultScene::Update() {
    fade_.Update();

    // ✅ スコア加算演出
    if (currentScore_ < targetScore_) {
        currentScore_++;
        scoreUI_->SetNumber(currentScore_);
    }
    scoreUI_->Update();

    if (input_->TriggerKey(DIK_RETURN) && fade_.GetState() == Fade::State::Stay) {
        fade_.StartFadeOut();
        fadeOutStarted_ = true;
        SetSceneNo(SCENE::Title); // タイトルへ戻る
    }

    if (fadeOutStarted_ && fade_.IsFinished()) {
        finished_ = true;
    }
}

void ResultScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    Sprite::PreDraw(dxCommon->GetCommandList());
    backgroundSprite_->Draw();
    resultSprite_->Draw();
	resultUI_->Draw();

    // ✅ スコア描画
    if (scoreUI_) {
        scoreUI_->Draw();
    }

    fade_.Draw();
    Sprite::PostDraw();
}

void ResultScene::Finalize() {}