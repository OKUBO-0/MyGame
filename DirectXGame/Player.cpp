#include "Player.h"

using namespace KamataEngine;

// コンストラクタ
Player::Player() {
}

// デストラクタ
Player::~Player() {
	delete playerModel_;
}

// 初期化
void Player::Initialize() {
	// Inputインスタンスの取得
	input_ = Input::GetInstance();

	// カメラの初期化
	camera_.Initialize();

	// プレイヤーモデルの読み込み
	playerModel_ = Model::CreateFromOBJ("octopus");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 10.0f, 0.0f, 0.0f };
}

// 更新
void Player::Update() {
	if (!canMove_) {
		// ワールド行列だけ更新して終了（回転も移動もしない）
		worldTransform_.UpdateMatrix();
		return;
	}

	// 入力による移動方向ベクトル
	Vector3 move = { 0.0f, 0.0f, 0.0f };

	// 追記：補間係数（0.1〜0.2程度が自然）
	const float rotateLerpFactor = 0.1f;

	if (input_->PushKey(DIK_W)) {
		move.z += speed_;
	}
	if (input_->PushKey(DIK_S)) {
		move.z -= speed_;
	}
	if (input_->PushKey(DIK_A)) {
		move.x -= speed_;
	}
	if (input_->PushKey(DIK_D)) {
		move.x += speed_;
	}

	if (move.x != 0.0f || move.z != 0.0f) {
		// 正規化
		float length = std::sqrt(move.x * move.x + move.z * move.z);
		move.x /= length;
		move.z /= length;

		// 移動
		worldTransform_.translation_.x += move.x * speed_;
		worldTransform_.translation_.z += move.z * speed_;

		// 目標角度（Z軸正方向を基準にしたラジアン）
		float targetAngle = std::atan2(move.x, move.z);

		// 現在角度
		float currentAngle = worldTransform_.rotation_.y;

		// ラジアンの差を -π ～ π に収める
		float delta = targetAngle - currentAngle;
		if (delta > 3.14159265f) {
			delta -= 2.0f * 3.14159265f;
		}
		if (delta < -3.14159265f) {
			delta += 2.0f * 3.14159265f;
		}

		// 線形補間（滑らかに回転）
		worldTransform_.rotation_.y += delta * rotateLerpFactor;
	}

	//// プレイヤーのワールド座標を取得
	//Vector3 playerPos = worldTransform_.translation_;

	//// プレイヤーの背後にカメラを配置
	//Vector3 cameraOffset = { 0.0f, 10.0f, -30.0f };
	//Vector3 cameraPos = {
	//	playerPos.x + cameraOffset.x,
	//	playerPos.y + cameraOffset.y,
	//	playerPos.z + cameraOffset.z
	//};

	//// カメラの座標を更新
	//camera_.translation_ = cameraPos;

	//// プレイヤーの位置を見るように回転（ビュー行列を作るときに見る点を決定する）
	//camera_.UpdateMatrix();  // translation_ や rotation_ に基づいて matView を再計算

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();
}

// 描画
void Player::Draw() {
	// モデルの描画
	playerModel_->Draw(worldTransform_, camera_);
}