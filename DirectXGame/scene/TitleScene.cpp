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
	backgroundSprite_ = Sprite::Create(backgroundSpriteHandle_, { 0.0f, 0.0f });
	backgroundSprite_->SetSize(Vector2(1280, 720));

	// タイトルスプライトの初期化
	titleSpriteHandle_ = TextureManager::Load("title/title.png");
	titleSprite_ = Sprite::Create(titleSpriteHandle_, { 0.0f, -300.0f });
	titleSprite_->SetSize(Vector2(1280, 720));

	// タイトルUIスプライトの初期化
	titleUISpriteHandle_ = TextureManager::Load("title/titleUI.png");
	titleUISprite = Sprite::Create(titleUISpriteHandle_, { 0.0f, 0.0f });
	titleUISprite->SetSize(Vector2(1280, 720));
	titleUISprite->SetColor({ 1, 1, 1, 0 });
}

void TitleScene::Update() {
	// シーン終了
	if (input_->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}

	// タイトルスプライトの今の座標を取得
	Vector2 pos = titleSprite_->GetPosition();

	// タイトル演出
	if (pos.y < 0.0f) {
		pos.y += 3.0f;
		if (pos.y > 0.0f) pos.y = 0.0f;
		titleSprite_->SetPosition(pos);
	}
	else {
		static float timer = 0.0f;
		timer += 0.02f;  // 点滅の速さ調整

		// sin波で0〜1の値を作る（0.5〜1.0に変換）
		float alpha = 0.5f + 0.5f * sinf(timer * 3.1415f * 2.0f);
		// hitEnter点滅
		titleUISprite->SetColor({ 1.0f, 1.0f, 1.0f, alpha });
	}
}

void TitleScene::Draw() {
	// DirectXCommon インスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// タイトルスプライトの描画
	backgroundSprite_->Draw();

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

	// タイトルスプライトの描画
	titleSprite_->Draw();

	// hitEnterスプライト
	titleUISprite->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}