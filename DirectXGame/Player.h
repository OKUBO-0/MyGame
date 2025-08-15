#pragma once
#include "Collider.h"

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

	AABB GetAABB() const;

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
};