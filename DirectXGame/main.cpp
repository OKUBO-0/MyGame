#include <Windows.h>
#include <KamataEngine.h>

#include "scene/SceneManager.h"
#include "scene/TitleScene.h"
#include "scene/GameScene.h"
#include "scene/ResultScene.h"

using namespace KamataEngine;

// Windowsアプリのエントリーポイント
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    // エンジン初期化
    Initialize(L"LE3C_04_オオクボ_タク");

    // DirectXCommonインスタンス取得
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // シーン管理
    SceneManager sceneManager;

    // シーン登録
    sceneManager.RegisterScene(SCENE::Title, []() { return std::make_unique<TitleScene>(); });
    sceneManager.RegisterScene(SCENE::Game, []() { return std::make_unique<GameScene>(); });
    sceneManager.RegisterScene(SCENE::Result, []() { return std::make_unique<ResultScene>(); });

    // 初期シーン設定
    sceneManager.ChangeScene(SCENE::Title);

    // メインループ
    while (true) {
        // エンジン更新（終了判定）
        if (Update()) break;

        // シーン更新
        sceneManager.Update();

        // 描画処理
        dxCommon->PreDraw();
        sceneManager.Draw();
        dxCommon->PostDraw();
    }

    // エンジン終了処理
    Finalize();

    return 0;
}