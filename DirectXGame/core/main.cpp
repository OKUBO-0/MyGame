#include <Windows.h>
#include <KamataEngine.h>
#include <chrono>

#include "SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

using namespace KamataEngine;

namespace EngineLayer {

void RegisterScenes(::SceneManager& sceneManager) {
    sceneManager.RegisterScene(::Scene::Title, []() { return std::make_unique<::TitleScene>(); });
    sceneManager.RegisterScene(::Scene::Game, []() { return std::make_unique<::GameScene>(); });
    sceneManager.RegisterScene(::Scene::Result, []() { return std::make_unique<::ResultScene>(); });
}

} // namespace EngineLayer

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    // エンジン初期化（ウィンドウタイトルを指定）
    Initialize(L"LE3C_04_オオクボ_タク");

    // フルスクリーン化（必要なら有効化）
    // WinApp::GetInstance()->SetFullscreen(true);

    // DirectX共通インスタンス取得（描画制御用）
    DirectXCommon* directXCommon = DirectXCommon::GetInstance();
    ImGuiManager* imguiManager = ImGuiManager::GetInstance();

    // シーン管理クラス生成
    SceneManager sceneManager;

    // 各シーンを登録（Title / Game / Result）
    EngineLayer::RegisterScenes(sceneManager);

    // 初期シーンをタイトルに設定
    sceneManager.ChangeScene(Scene::Title);

    auto lastFrameTime = std::chrono::steady_clock::now();
    float fps = 0.0f;
    float frameTimeMs = 0.0f;

    // メインループ（ゲームが終了するまで繰り返し）
    while (true) {
        // エンジン更新（終了判定。trueが返ればループ終了）
        if (Update()) { break; }

        const auto now = std::chrono::steady_clock::now();
        const std::chrono::duration<float> delta = now - lastFrameTime;
        lastFrameTime = now;
        frameTimeMs = delta.count() * 1000.0f;
        fps = frameTimeMs > 0.0f ? 1000.0f / frameTimeMs : 0.0f;

        imguiManager->Begin();

//#ifdef DEBUG
        ImGui::SetNextWindowPos(ImVec2(16.0f, 16.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.75f);
        if (ImGui::Begin("Performance", nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("FPS: %.1f", fps);
            ImGui::Text("Frame Time: %.2f ms", frameTimeMs);
        }
        ImGui::End();
//#endif // DEBUG

        // 現在のシーンを更新
        sceneManager.Update();

        imguiManager->End();

        // 描画処理開始
        directXCommon->PreDraw();

        // 現在のシーンを描画
        sceneManager.Draw();

        imguiManager->Draw();

        // 描画処理終了
        directXCommon->PostDraw();
    }

    // エンジン終了処理（リソース解放など） 
    Finalize();

    return 0;
}
