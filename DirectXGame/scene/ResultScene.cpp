#include "ResultScene.h"
using namespace KamataEngine;

ResultScene::ResultScene() {}
ResultScene::~ResultScene() {
    // 動的に生成したスプライトやUIを解放
    delete backgroundSprite_;
    delete resultSprite_;
    delete resultUI_;
    delete scoreUI_;
}

void ResultScene::Initialize() {
    // 各種シングルトンの取得（描画・入力・音声）
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    // 背景スプライト生成（黒背景）
    uint32_t blackTex = TextureManager::Load("color/black.png");
    backgroundSprite_ = Sprite::Create(blackTex, { 0,0 });
    backgroundSprite_->SetSize({ 1280,720 });

    // リザルト画面用スプライト生成
    uint32_t resultTex = TextureManager::Load("result/result.png");
    resultSprite_ = Sprite::Create(resultTex, { 0,0 });
    resultSprite_->SetSize({ 1280,720 });

    // 終了UIスプライト生成
    uint32_t uiTex = TextureManager::Load("result/finish_ui.png");
    resultUI_ = Sprite::Create(uiTex, { 0,0 });
    resultUI_->SetSize({ 1280,720 });

    // スコアUI生成（数値表示用）
    scoreUI_ = new Score();
    scoreUI_->Initialize();
    scoreUI_->SetNumber(0);              // 初期値は0
    scoreUI_->SetPosition({ 500, 200 }); // 画面中央付近に配置
    scoreUI_->SetScale(2.0f);            // 倍率を大きめに設定

    // スコア演出用変数
    currentScore_ = 0;                   // 現在表示中のスコア
    targetScore_ = GameData::totalEXP;   // 最終的に表示するスコア（総獲得EXP）

    // フェード初期化
    fade_.Initialize();
    fadeOutStarted_ = false;
}

void ResultScene::Update() {
    // フェード更新（常に先頭で処理）
    fade_.Update();

    // スコア加算演出（徐々に最終スコアまで増加させる）
    if (currentScore_ < targetScore_) {
        currentScore_++;
        scoreUI_->SetNumber(currentScore_);
    }
    scoreUI_->Update();

    // Enterキーでタイトルへ戻る（フェードアウト開始）
    if (input_->TriggerKey(DIK_RETURN) && fade_.GetState() == Fade::State::Stay) {
        fade_.StartFadeOut();
        fadeOutStarted_ = true;
        SetSceneNo(SCENE::Title);
    }

    // フェードアウト完了後、シーン終了フラグを立てる
    if (fadeOutStarted_ && fade_.IsFinished()) {
        finished_ = true;
    }
}

void ResultScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // スプライト描画開始
    Sprite::PreDraw(dxCommon->GetCommandList());

    // 背景・リザルト画面・UIを描画
    backgroundSprite_->Draw();
    resultSprite_->Draw();
    resultUI_->Draw();

    // スコア描画（加算演出で更新された値を表示）
    if (scoreUI_) {
        scoreUI_->Draw();
    }

    // フェード描画（シーン遷移演出）
    fade_.Draw();

    // スプライト描画終了
    Sprite::PostDraw();
}

void ResultScene::Finalize() {
    // 特別な終了処理は不要（リソース解放はデストラクタで対応）
}