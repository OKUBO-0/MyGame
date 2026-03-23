#include "SceneManager.h"

namespace EngineLayer {

using SceneFactory = std::unordered_map<Scene, std::function<std::unique_ptr<IScene>()>>;

std::unique_ptr<IScene> CreateSceneIfRegistered(const SceneFactory& sceneFactory, Scene scene) {
    auto it = sceneFactory.find(scene);
    if (it == sceneFactory.end()) {
        return nullptr;
    }

    return it->second();
}

constexpr Scene kInitialScene = Scene::Title;

} // namespace EngineLayer

/// <summary>
/// コンストラクタ：初期シーンをタイトルに設定
/// </summary>
SceneManager::SceneManager() : currentSceneNo_(EngineLayer::kInitialScene) {}

/// <summary>
/// デストラクタ：unique_ptrによりリソースは自動解放される
/// </summary>
SceneManager::~SceneManager() = default;

void SceneManager::RegisterScene(Scene scene, std::function<std::unique_ptr<IScene>()> createFunc) {
    // シーン生成関数を登録（sceneFactory_ に紐付け）
    sceneFactory_[scene] = std::move(createFunc);
}

void SceneManager::ChangeScene(Scene scene) {
    // 登録済みのシーンであれば切り替えを実行
    currentScene_ = EngineLayer::CreateSceneIfRegistered(sceneFactory_, scene);
    if (currentScene_) {
        currentScene_->Initialize();

        // 現在のシーン番号を更新
        currentSceneNo_ = scene;

        // 静的変数にも反映（他クラスから参照可能にするため）
        IScene::SetStaticSceneNo(scene);
    }
}

void SceneManager::Update() {
    // 現在のシーンが存在しない場合は処理しない
    if (!currentScene_) { return; }

    // 現在のシーンを更新
    currentScene_->Update();

    // シーンが終了状態になったら次のシーンへ切り替え
    if (currentScene_->IsFinished()) {
        Scene next = currentScene_->GetSceneNo();
        ChangeScene(next);
    }
}

void SceneManager::Draw() {
    // 現在のシーンが存在する場合のみ描画
    if (currentScene_) {
        currentScene_->Draw();
    }
}
