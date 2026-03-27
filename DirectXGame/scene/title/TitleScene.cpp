#include "TitleScene.h"
#include <cmath>

using namespace KamataEngine;

namespace DirectXGame {

void TitleScene::Initialize() {
    // --- 各種シングルトン取得（描画・入力・音声） ---
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    // --- タイトル画面用BGMと効果音を読み込み ---
    titleBGMHandle_ = audio_->LoadWave("audio/bgm/title.wav");
    selectSEHandle_ = audio_->LoadWave("audio/se/se_pause.wav");
    decideSEHandle_ = audio_->LoadWave("audio/se/se_exp.wav");

    // --- タイトルロゴスプライト生成 ---
    uint32_t titleTex = TextureManager::Load("ui/title/title.png");
    titleSprite_ = std::unique_ptr<Sprite>(Sprite::Create(titleTex, { 0, 0 }));
    titleSprite_->SetSize({ 1280, 720 });

    // --- カーソルスプライト生成 ---
    uint32_t cursorTex = TextureManager::Load("ui/title/cursor.png");
    cursorSprite_ = std::unique_ptr<Sprite>(Sprite::Create(cursorTex, { 0, 0 }));
    cursorSprite_->SetSize({ 1280, 720 });

    // ガイド画面
    uint32_t guideUITex = TextureManager::Load("ui/title/guideUI.png");
    guideSprite_ = std::unique_ptr<Sprite>(Sprite::Create(guideUITex, { 0, 0 }));
	guideSprite_->SetSize({1280, 720});

    // モデル生成（天球モデルを読み込み、背景の空を表現）
    playerModel_.reset(Model::CreateFromOBJ("octopus"));

    // 天球生成
	skyDome_ = std::make_unique<SkyDome>();
	skyDome_->Initialize();

    // ワールド変換初期化
    worldTransform_.Initialize();
    // モデルの向き（反対方向）
    worldTransform_.rotation_ = { 0.0f, -2.618f, 0.0f };
    // モデルの大きさ
    worldTransform_.scale_ = { 3.0f, 3.0f, 3.0f };
    // モデルの位置（右下に寄せる）
    worldTransform_.translation_ = { 20.0f, -10.0f, 0.0f };
    // 行列更新
    worldTransform_.UpdateMatrix();

    // --- 遷移演出用変数の初期化 ---
    curtain_.Initialize();
    curtain_.StartOpen(20.0f);
    curtainOpening_ = true;

    // --- カメラ設定 --- 
    camera_.Initialize();
    camera_.rotation_ = { 0.0f, 0.0f, 0.0f }; 
    camera_.translation_ = { 0.0f, 0.0f, -50.0f }; 
    camera_.UpdateMatrix();
}

void TitleScene::Update(float deltaTime) {
    // --- 遷移演出更新（常に先頭で処理） ---
    curtain_.Update(deltaTime);

    // カーテン開き中は操作禁止
    if (curtainOpening_) {
        if (curtain_.GetState() == CurtainTransition::State::kNone) {
            curtainOpening_ = false; // 開き終わった
        }
        return; // 入力処理を止める
    }

    // --- BGM再生（ループ再生、未再生なら開始） ---
    if (!audio_->IsPlaying(titleBGMHandle_)) {
        titleBGMHandle_ = audio_->PlayWave(titleBGMHandle_, true, 0.1f);
    }

    // ガイド表示中は ESC で閉じる
    if (guideActive_) {
        if (input_->TriggerKey(DIK_ESCAPE)) {
            guideActive_ = false;
            if (decideSEHandle_ != 0) {
                audio_->PlayWave(decideSEHandle_, false, 1.0f);
            }
        }
        return; // メニュー操作は無効
    }

    // --- メニュー選択（W / S） ---
    int32_t previousMenuIndex = menuIndex_;
    if (input_->TriggerKey(DIK_W)) {
        menuIndex_ = std::max<int32_t>(0, menuIndex_ - 1);
    }
    if (input_->TriggerKey(DIK_S)) {
        menuIndex_ = std::min<int32_t>(2, menuIndex_ + 1);
    }
    if (menuIndex_ != previousMenuIndex) {
        if (selectSEHandle_ != 0) {
            audio_->PlayWave(selectSEHandle_, false, 1.0f);
        }
    }

    // --- カーソル位置更新 ---
    switch (menuIndex_) {
    case 0: cursorSprite_->SetPosition({ 0, 0 }); break; // Play
    case 1: cursorSprite_->SetPosition({ 0, 120 }); break; // Guide
    case 2: cursorSprite_->SetPosition({ 0, 240 }); break; // Quit
    }

    // --- 決定（SPACE / ENTER） ---
    if (input_->TriggerKey(DIK_SPACE)) {
        if (decideSEHandle_ != 0) {
            audio_->PlayWave(decideSEHandle_, false, 1.0f);
        }
        switch (menuIndex_) {
        case 0: // Play
            if (curtain_.GetState() == CurtainTransition::State::kNone) {
                curtain_.StartClose();
                curtainStarted_ = true;
                SetSceneNo(Scene::Game);
            }
            break;

        case 1: // Guide
            guideActive_ = true;
            break;

        case 2: // Quit
            PostQuitMessage(0); // Windowsアプリ終了
            break;
        }
    }

    // --- 遷移演出完了 → シーン終了処理 ---
    if (curtainStarted_ && curtain_.IsFinished()) {
        audio_->StopWave(titleBGMHandle_);
        finished_ = true;
    }

    // --- プレイヤーモデルのアニメーション（回転 + 浮遊 + 左右揺れ） ---

    // 時間経過
    animationTime_ += deltaTime;

    // 回転（Y軸）
    worldTransform_.rotation_.y += deltaTime * 0.625f;

    // 上下浮遊（サイン波）
    float baseY = -10.0f; // Initialize() で設定した初期位置
    float floatY = sinf(animationTime_ * 1.875f) * 1.5f;
    worldTransform_.translation_.y = baseY + floatY;

    // 左右揺れ（サイン波）
    float baseX = 20.0f; // Initialize() で設定した初期位置
    float floatX = sinf(animationTime_ * 1.25f) * 1.0f;
    worldTransform_.translation_.x = baseX + floatX;

    // 行列更新
    worldTransform_.UpdateMatrix();
}

void TitleScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // --- 背景描画 ---
    Sprite::PreDraw(dxCommon->GetCommandList());
    Sprite::PostDraw();

    // --- モデル描画（タイトル演出用） ---
    dxCommon_->ClearDepthBuffer();
    Model::PreDraw();
    if (playerModel_) {
        playerModel_->Draw(worldTransform_, camera_);
    }
	skyDome_->Draw();
    Model::PostDraw();

    // --- UI・フェード描画 ---
    Sprite::PreDraw(dxCommon->GetCommandList());
    titleSprite_->Draw();
    // ガイド画面表示中
    if (guideActive_) {
        guideSprite_->Draw();
        Sprite::PostDraw();
        return;
    }
    cursorSprite_->Draw();
    curtain_.Draw();
    Sprite::PostDraw();
}

void TitleScene::Finalize() {
    // --- 特別な終了処理は不要（リソース解放はデストラクタで対応） ---
}

} // namespace DirectXGame
