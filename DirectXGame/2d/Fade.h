#pragma once

#include <KamataEngine.h>

class Fade {
public:
	enum class State {
		None,
		FadeIn,
		Stay,
		FadeOut,
	};

	// コンストラクタ
	Fade();

	// デストラクタ
	~Fade();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// フェードイン開始
	void StartFadeIn(float speed = 0.02f);

	// フェードアウト開始
	void StartFadeOut(float speed = 0.02f);

	// 現在の状態取得
	State GetState() const { return state_; }

	// フェード完了判定
	bool IsFinished() const { return finished_; }

private:
	KamataEngine::Sprite* fadeSprite_ = nullptr;
	float alpha_ = 1.0f;
	float speed_ = 0.02f;
	State state_ = State::None;
	bool finished_ = false;
};
