#include "SceneManager.h"

/// <summary>
/// コンストラクタ：初期シーンをタイトルに設定
/// </summary>
SceneManager::SceneManager() : currentSceneNo_(Scene::Title) {}

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
    auto it = sceneFactory_.find(scene);
    if (it != sceneFactory_.end()) {
        // 新しいシーンを生成
        currentScene_ = it->second();
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