#pragma once
#include "IScene.h"

#include "../SkyDome.h"
#include "../Player.h"
#include "../Enemy.h"

#include "../Fade.h"

#include "../Collider.h"

#include <KamataEngine.h>

class GameScene : public IScene
{
public:
	// コンストラクタ
	GameScene();

	// デストラクタ
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// 終了フラグ
	bool IsFinished() const { return finished_; }

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	// 天球
	SkyDome* skyDome_ = nullptr;

	// プレイヤー
	Player* player_ = nullptr;

	// 敵
	Enemy* enemy_ = nullptr;

	// フェード
	Fade fade_;
	bool fadeOutStarted_ = false;

	// 終了フラグ
	bool finished_ = false;
};