#include "KamataEngine.h"
#include "scene/SceneManager.h"
#include "scene/TitleScene.h"
#include "scene/GameScene.h"

#include <Windows.h>

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	Initialize(L"LE3C_04_オオクボ_タク");

	// DirectXCommonのインスタンスを取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	SceneManager sceneManager;

	// シーン登録
	sceneManager.RegisterScene(SceneName::Title, []() { return std::make_unique<TitleScene>(); });
	sceneManager.RegisterScene(SceneName::Game, []() { return std::make_unique<GameScene>(); });

	// 初期シーンをタイトルに設定
	sceneManager.ChangeScene(SceneName::Title);

	// メインループ
	while (true) {
		// エンジンの更新
		if (Update()) {
			break;
		}

		// シーン更新
		sceneManager.Update();

		// 描画開始
		dxCommon->PreDraw();

		// シーンの描画
		sceneManager.Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// エンジンの終了処理
	Finalize();

	return 0;
}