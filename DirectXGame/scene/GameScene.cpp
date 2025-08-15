#include "GameScene.h"

using namespace KamataEngine;

// コンストラクタ
GameScene::GameScene() {
}

// デストラクタ
GameScene::~GameScene() {
	delete skyDome_;
	delete player_;
	delete enemy_;
}

void GameScene::Initialize() {
	// DirectXCommonインスタンスの取得
	dxCommon_ = DirectXCommon::GetInstance();
	// Inputインスタンスの取得
	input_ = Input::GetInstance();
	// Audioインスタンスの取得
	audio_ = Audio::GetInstance();

	// カメラ
	camera_.Initialize();

	// 天球
	skyDome_ = new SkyDome();
	skyDome_->Initialize();

	// プレイヤーの初期化
	player_ = new Player();
	player_->Initialize();

	// 敵の初期化
	enemy_ = new Enemy();
	enemy_->Initialize();

	// フェードの初期化
	fade_.Initialize();
	fadeOutStarted_ = false;

	// 当たり判定サイズ読み込み
	CollisionManager::GetInstance()->LoadHitBoxCSV("csv/HitBoxData.csv");
}

void GameScene::Update() {
	// フェードの更新
	fade_.Update();

	// シーン終了
	if (input_->TriggerKey(DIK_ESCAPE) && fade_.GetState() == Fade::State::Stay) {
		fade_.StartFadeOut();
		fadeOutStarted_ = true;
	}

	// フェードアウト完了でシーン終了
	if (fadeOutStarted_ && fade_.IsFinished()) {
		finished_ = true;
	}

	// プレイヤーの更新
	player_->Update();

	// 敵の更新
	enemy_->Update();

	// 当たり判定
	auto& playerHit = CollisionManager::GetInstance()->GetHitBoxData("Player");
	auto& enemyHit = CollisionManager::GetInstance()->GetHitBoxData("Enemy");

	if (CollisionManager::GetInstance()->CheckCollisionCircle(
		player_->GetPosition(), playerHit.radius,
		enemy_->GetPosition(), enemyHit.radius)) {
		printf("プレイヤーと敵が衝突しました！\n");
		player_->SetCanMove(false); // 移動禁止
	}
	else {
		player_->SetCanMove(true);  // 衝突していなければ移動可能
	}
}

void GameScene::Draw() {
	// DirectXCommon インスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// スプライト描画後処理
	Sprite::PostDraw();

	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dモデル描画
	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 天球の描画
	skyDome_->Draw();

	// プレイヤーの描画
	player_->Draw();

	// 敵の描画
	enemy_->Draw();

	// 3Dモデル描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// フェードの描画
	fade_.Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}