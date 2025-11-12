#pragma once

#include <KamataEngine.h>

class SkyDome
{
public:
	// コンストラクタ
	SkyDome();

	// デストラクタ
	~SkyDome();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// カメラ
	KamataEngine::Camera camera_;

	// 天球モデル
	KamataEngine::Model* skyModel_ = nullptr;
};