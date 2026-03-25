#include "SceneManager.h"

namespace DirectXGame {

using SceneFactory = std::unordered_map<Scene, std::function<std::unique_ptr<IScene>(const std::shared_ptr<GameSessionContext>&)>>;

std::unique_ptr<IScene> CreateSceneIfRegistered(
    const SceneFactory& sceneFactory, Scene scene, const std::shared_ptr<GameSessionContext>& sessionContext) {
    auto it = sceneFactory.find(scene);
    if (it == sceneFactory.end()) {
        return nullptr;
    }

    return it->second(sessionContext);
}

constexpr Scene kInitialScene = Scene::Title;

/// <summary>
/// コンストラクタ：初期シーンをタイトルに設定
/// </summary>
SceneManager::SceneManager()
    : currentSceneNo_(DirectXGame::kInitialScene),
      sessionContext_(std::make_shared<GameSessionContext>()) {}

/// <summary>
/// デストラクタ：unique_ptrによりリソースは自動解放される
/// </summary>
SceneManager::~SceneManager() = default;

void SceneManager::RegisterScene(
    Scene scene,
    const std::function<std::unique_ptr<IScene>(const std::shared_ptr<GameSessionContext>&)>& createFunc) {
    // シーン生成関数を登録（sceneFactory_ に紐付け）
    sceneFactory_[scene] = createFunc;
}

void SceneManager::ChangeScene(Scene scene) {
    // 登録済みのシーンであれば切り替えを実行
    currentScene_ = DirectXGame::CreateSceneIfRegistered(sceneFactory_, scene, sessionContext_);
    if (currentScene_) {
        currentScene_->Initialize();

        // 現在のシーン番号を更新
        currentSceneNo_ = scene;
    }
}

void SceneManager::Update() {
    // 現在のシーンが存在しない場合は処理しない
    if (!currentScene_) { return; }

    // 現在のシーンを更新
    currentScene_->Update();

    // シーンが終了状態になったら次のシーンへ切り替え
    if (currentScene_->IsFinished()) {
        Scene next = currentScene_->GetNextSceneNo();
        ChangeScene(next);
    }
}

void SceneManager::Draw() {
    // 現在のシーンが存在する場合のみ描画
    if (currentScene_) {
        currentScene_->Draw();
    }
}

} // namespace DirectXGame
