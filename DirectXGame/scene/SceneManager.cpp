#include "SceneManager.h"

/// <summary>
/// コンストラクタ：初期シーンをタイトルに設定
/// </summary>
SceneManager::SceneManager() : currentSceneNo_(Scene::Title) {}

SceneManager::~SceneManager() {}

void SceneManager::RegisterScene(Scene scene, std::function<std::unique_ptr<IScene>()> createFunc) {
    /// <summary>
    /// シーン生成関数を登録（sceneFactory_ に紐付け）
    /// 指定されたシーン番号から対応するシーンを生成できるようにする
    /// </summary>
    sceneFactory_[scene] = createFunc;
}

void SceneManager::ChangeScene(Scene scene) {
    /// <summary>
    /// 登録済みのシーンであれば切り替えを実行
    /// </summary>
    if (sceneFactory_.find(scene) != sceneFactory_.end()) {
        // 新しいシーンを生成
        currentScene_ = sceneFactory_[scene]();
        currentScene_->Initialize();

        // 現在のシーン番号を更新
        currentSceneNo_ = scene;

        // 静的変数にも反映（他クラスから参照可能にするため）
        IScene::SetStaticSceneNo(scene);
    }
}

void SceneManager::Update() {
    /// <summary>
    /// 現在のシーンが存在しない場合は処理しない
    /// </summary>
    if (!currentScene_) { return; }

    /// <summary>
    /// 現在のシーンを更新
    /// </summary>
    currentScene_->Update();

    /// <summary>
    /// シーンが終了状態になったら次のシーンへ切り替え
    /// </summary>
    if (currentScene_->IsFinished()) {
        Scene next = currentScene_->GetSceneNo();
        ChangeScene(next);
    }
}

void SceneManager::Draw() {
    /// <summary>
    /// 現在のシーンが存在する場合のみ描画
    /// </summary>
    if (currentScene_) {
        currentScene_->Draw();
    }
}