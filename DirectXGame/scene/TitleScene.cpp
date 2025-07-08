#include "TitleScene.h"

using namespace KamataEngine;

// コンストラクタ
TitleScene::TitleScene() {
}

// デストラクタ
TitleScene::~TitleScene() {
	delete titleSprite_;
}

void TitleScene::Initialize() {
	// DirectXCommonインスタンスの取得
	dxCommon_ = DirectXCommon::GetInstance();
	// Inputインスタンスの取得
	input_ = Input::GetInstance();
	// Audioインスタンスの取得
	audio_ = Audio::GetInstance();

	// カメラ
	camera_.Initialize();

	// 背景スプライトの初期化
	backgroundSpriteHandle_ = TextureManager::Load("color/black.png");
	backgroundSprite_ = Sprite::Create(backgroundSpriteHandle_, {0.0f, 0.0f});
	backgroundSprite_->SetSize(Vector2(1280, 720));

	// タイトルスプライトの初期化
	titleSpriteHandle_ = TextureManager::Load("color/black.png");
	titleSprite_ = Sprite::Create(titleSpriteHandle_, {0.0f, 0.0f});
	titleSprite_->SetSize(Vector2(1280, 720));
}

void TitleScene::Update() {
	if (input_->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}
}

void TitleScene::Draw() {
	// DirectXCommon インスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// タイトルスプライトの描画
	titleSprite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dモデル描画
	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデル描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}