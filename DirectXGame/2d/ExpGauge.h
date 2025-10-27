#pragma once
#include <KamataEngine.h>

class ExpGauge
{
public:
	ExpGauge();
	~ExpGauge();
	// ==============================
	// 基本処理
	// ==============================
	void Initialize();   // 初期化
	void Update();       // 更新
	void Draw();         // 描画
	// ==============================
	// 位置とスケール設定
	// ==============================
	void SetPosition(const KamataEngine::Vector2& pos);
	void SetScale(float scale);
};