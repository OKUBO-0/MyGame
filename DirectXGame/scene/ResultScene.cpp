#include "ResultScene.h"
using namespace KamataEngine;

ResultScene::ResultScene() {}

ResultScene::~ResultScene() {
    /// <summary>
    /// 動的に生成したスプライトやUIを解放
    /// </summary>
    delete backgroundSprite_;
    delete resultSprite_;
    delete resultUI_;
    delete scoreUI_;
}

void ResultScene::Initialize() {
    /// <summary>
    /// 各種シングルトンの取得（描画・入力・音声）
    /// </summary>
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    /// <summary>
    /// 背景スプライト生成（黒背景）
    /// </summary>
    uint32_t blackTex = TextureManager::Load("color/black.png");
    backgroundSprite_ = Sprite::Create(blackTex, { 0,0 });
    backgroundSprite_->SetSize({ 1280,720 });

    /// <summary>
    /// リザルト画面用スプライト生成
    /// </summary>
    uint32_t resultTex = TextureManager::Load("result/result.png");
    resultSprite_ = Sprite::Create(resultTex, { 0,0 });
    resultSprite_->SetSize({ 1280,720 });

    /// <summary>
    /// 終了UIスプライト生成
    /// </summary>
    uint32_t uiTex = TextureManager::Load("result/finish_ui.png");
    resultUI_ = Sprite::Create(uiTex, { 0,0 });
    resultUI_->SetSize({ 1280,720 });

    /// <summary>
    /// スコアUI生成（数値表示用）
    /// </summary>
    scoreUI_ = new Score();
    scoreUI_->Initialize();
    scoreUI_->SetNumber(0);
    scoreUI_->SetPosition({ 500, 200 });
    scoreUI_->SetScale(2.0f);

    /// <summary>
    /// スコア演出用変数
    /// </summary>
    currentScore_ = 0;
    targetScore_ = GameData::totalExp;

    /// <summary>
    /// フェード初期化
    /// </summary>
    fade_.Initialize();
    fadeOutStarted_ = false;
}

void ResultScene::Update() {
    /// <summary>
    /// フェード更新（常に先頭で処理）
    /// </summary>
    fade_.Update();

    /// <summary>
    /// スコア加算演出（徐々に最終スコアまで増加させる）
    /// </summary>
    if (currentScore_ < targetScore_) {
        currentScore_ += 1;
        scoreUI_->SetNumber(currentScore_);
    }
    scoreUI_->Update();

    /// <summary>
    /// Enterキーでタイトルへ戻る（フェードアウト開始）
    /// </summary>
    if (input_->TriggerKey(DIK_RETURN) && fade_.GetState() == Fade::State::kStay) {
        fade_.StartFadeOut();
        fadeOutStarted_ = true;
        SetSceneNo(Scene::Title);
    }

    /// <summary>
    /// フェードアウト完了後、シーン終了フラグを立てる
    /// </summary>
    if (fadeOutStarted_ && fade_.IsFinished()) {
        finished_ = true;
    }
}

void ResultScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    /// <summary>
    /// スプライト描画開始
    /// </summary>
    Sprite::PreDraw(dxCommon->GetCommandList());

    /// <summary>
    /// 背景・リザルト画面・UIを描画
    /// </summary>
    backgroundSprite_->Draw();
    resultSprite_->Draw();
    resultUI_->Draw();

    /// <summary>
    /// スコア描画（加算演出で更新された値を表示）
    /// </summary>
    if (scoreUI_) {
        scoreUI_->Draw();
    }

    /// <summary>
    /// フェード描画（シーン遷移演出）
    /// </summary>
    fade_.Draw();

    /// <summary>
    /// スプライト描画終了
    /// </summary>
    Sprite::PostDraw();
}

void ResultScene::Finalize() {
    /// <summary>
    /// 特別な終了処理は不要（リソース解放はデストラクタで対応）
    /// </summary>
}