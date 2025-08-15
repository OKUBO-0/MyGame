#pragma once

#include <KamataEngine.h>

class Player
{
public:
	// コンストラクタ
	Player();

	// デストラクタ
	~Player();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }
	void SetCanMove(bool canMove) { canMove_ = canMove; }

private:
	// 入力インスタンス
	KamataEngine::Input* input_ = nullptr;
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// カメラ
	KamataEngine::Camera camera_;

	// プレイヤーモデル
	KamataEngine::Model* playerModel_ = nullptr;

	float speed_ = 0.3f;

	bool canMove_ = true;
};