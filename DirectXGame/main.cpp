#include <Windows.h>
#include <KamataEngine.h>

#include "scene/SceneManager.h"
#include "scene/TitleScene.h"
#include "scene/GameScene.h"
#include "scene/ResultScene.h"

using namespace KamataEngine;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    // エンジン初期化（ウィンドウタイトルを指定）
    Initialize(L"LE3C_04_オオクボ_タク");

    // フルスクリーン化（必要なら有効化）
    // WinApp::GetInstance()->SetFullscreen(true);

    // DirectX共通インスタンス取得（描画制御用）
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // シーン管理クラス生成
    SceneManager sceneManager;

    // 各シーンを登録（Title / Game / Result）
    sceneManager.RegisterScene(SCENE::Title, []() { return std::make_unique<TitleScene>(); });
    sceneManager.RegisterScene(SCENE::Game, []() { return std::make_unique<GameScene>(); });
    sceneManager.RegisterScene(SCENE::Result, []() { return std::make_unique<ResultScene>(); });

    // 初期シーンをタイトルに設定
    sceneManager.ChangeScene(SCENE::Title);

    // メインループ（ゲームが終了するまで繰り返し）
    while (true) {
        // エンジン更新（終了判定。trueが返ればループ終了）
        if (Update()) break;

        // 現在のシーンを更新
        sceneManager.Update();

        // 描画処理開始
        dxCommon->PreDraw();

        // 現在のシーンを描画
        sceneManager.Draw();

        // 描画処理終了
        dxCommon->PostDraw();
    }

    // エンジン終了処理（リソース解放など）
    Finalize();

    return 0;
}