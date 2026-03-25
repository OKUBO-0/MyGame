#include "ResultScene.h"
using namespace KamataEngine;

namespace DirectXGame {

void ResultScene::Initialize() {
    // --- 各種シングルトン取得（描画・入力・音声） ---
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    selectSEHandle_ = audio_->LoadWave("audio/se/se_pause.wav");
    countupSEHandle_ = audio_->LoadWave("audio/se/se_pause.wav");

    // --- 背景スプライト生成（黒背景） ---
    uint32_t blackTex = TextureManager::Load("textures/color/black.png");
    backgroundSprite_ = std::unique_ptr<Sprite>(Sprite::Create(blackTex, { 0,0 }));
    backgroundSprite_->SetSize({ 1280, 720 });

    // --- リザルト画面用スプライト生成 ---
    uint32_t resultTex = TextureManager::Load("ui/result/Result.png");
    resultSprite_ = std::unique_ptr<Sprite>(Sprite::Create(resultTex, { 0,0 }));
    resultSprite_->SetSize({ 1280, 720 });

    // --- 終了UIスプライト生成 ---
    uint32_t uiTex = TextureManager::Load("ui/result/finish_ui.png");
    resultUI_ = std::unique_ptr<Sprite>(Sprite::Create(uiTex, { 0,0 }));
    resultUI_->SetSize({ 1280, 720 });

    // 経験値
    expUI_ = std::make_unique<Score>();
    expUI_->Initialize();
    expUI_->SetNumber(0);
    expUI_->SetPosition({ 500, 200 });
    expUI_->SetScale(2.0f);

    // レベル
    levelUI_ = std::make_unique<Score>();
    levelUI_->Initialize();
    levelUI_->SetNumber(sessionContext_ ? sessionContext_->resultData.finalLevel : 0);
    levelUI_->SetPosition({ 500, 300 });
    levelUI_->SetScale(2.0f);

    // キル数
    killUI_ = std::make_unique<Score>();
    killUI_->Initialize();
    killUI_->SetNumber(sessionContext_ ? sessionContext_->resultData.totalKillCount : 0);
    killUI_->SetPosition({ 500, 400 });
    killUI_->SetScale(2.0f);

    // --- スコア演出用変数 ---
    currentExp_ = 0;
    targetExp_ = sessionContext_ ? sessionContext_->resultData.totalExp : 0;

    currentLevel_ = 0;
    targetLevel_ = sessionContext_ ? sessionContext_->resultData.finalLevel : 0;

    currentKill_ = 0;
    targetKill_ = sessionContext_ ? sessionContext_->resultData.totalKillCount : 0;

    expUI_->SetNumber(0);
    levelUI_->SetNumber(0);
    killUI_->SetNumber(0);

    // --- フェード初期化 ---
    fade_.Initialize();
    fadeOutStarted_ = false;
}

void ResultScene::Update() {
    // --- フェード更新（常に先頭で処理） ---
    fade_.Update();
    if (countupSECooldown_ > 0) {
        --countupSECooldown_;
    }

    // --- スコア加算演出（徐々に最終スコアまで増加させる） ---
    if (currentExp_ < targetExp_) {
        currentExp_ += 1;
        expUI_->SetNumber(currentExp_);
        if (countupSEHandle_ != 0 && countupSECooldown_ == 0) {
            audio_->PlayWave(countupSEHandle_, false, 0.25f);
            countupSECooldown_ = 6;
        }
    }

    // --- レベル加算演出 ---
    if (currentLevel_ < targetLevel_) {
        currentLevel_ += 1;
        levelUI_->SetNumber(currentLevel_);
        if (countupSEHandle_ != 0 && countupSECooldown_ == 0) {
            audio_->PlayWave(countupSEHandle_, false, 0.25f);
            countupSECooldown_ = 6;
        }
    }

    // --- キル数加算演出 ---
    if (currentKill_ < targetKill_) {
        currentKill_ += 1;
        killUI_->SetNumber(currentKill_);
        if (countupSEHandle_ != 0 && countupSECooldown_ == 0) {
            audio_->PlayWave(countupSEHandle_, false, 0.25f);
            countupSECooldown_ = 6;
        }
    }

    expUI_->Update();
    levelUI_->Update();
    killUI_->Update();

    // --- Enterキーでタイトルへ戻る（フェードアウト開始） ---
    if (input_->TriggerKey(DIK_SPACE) && fade_.GetState() == Fade::State::kStay) {
        if (selectSEHandle_ != 0) {
            audio_->PlayWave(selectSEHandle_, false, 1.0f);
        }
        fade_.StartFadeOut();
        fadeOutStarted_ = true;
        SetSceneNo(Scene::Title);
    }

    // --- フェードアウト完了後、シーン終了フラグを立てる ---
    if (fadeOutStarted_ && fade_.IsFinished()) {
        finished_ = true;
    }
}

void ResultScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // --- スプライト描画開始 ---
    Sprite::PreDraw(dxCommon->GetCommandList());

    // --- 背景・リザルト画面・UIを描画 ---
    backgroundSprite_->Draw();
    resultSprite_->Draw();
    resultUI_->Draw();

    // --- スコア描画（加算演出で更新された値を表示） ---
    if (expUI_) {
        expUI_->Draw();
    }
    if (levelUI_) {
        levelUI_->Draw();
	}
    if (killUI_) {
		killUI_->Draw();
    }

    // --- フェード描画（シーン遷移演出） ---
    fade_.Draw();

    // --- スプライト描画終了 ---
    Sprite::PostDraw();
}

void ResultScene::Finalize() {
    // --- 特別な終了処理は不要（リソース解放はデストラクタで対応） ---
}

} // namespace DirectXGame
