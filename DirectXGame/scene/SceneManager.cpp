#include "SceneManager.h"

SceneManager::SceneManager() : currentSceneNo_(SCENE::Title) {}
SceneManager::~SceneManager() {}

void SceneManager::RegisterScene(SCENE scene, std::function<std::unique_ptr<IScene>()> createFunc) {
    sceneFactory_[scene] = createFunc;
}

void SceneManager::ChangeScene(SCENE scene) {
    if (sceneFactory_.find(scene) != sceneFactory_.end()) {
        currentScene_ = sceneFactory_[scene]();
        currentScene_->Initialize();
        currentSceneNo_ = scene;
        IScene::SetStaticSceneNo(scene);
    }
}

void SceneManager::Update() {
    if (!currentScene_) return;

    currentScene_->Update();

    if (currentScene_->IsFinished()) {
        SCENE next = currentScene_->GetSceneNo();
        ChangeScene(next);
    }
}

void SceneManager::Draw() {
    if (currentScene_) {
        currentScene_->Draw();
    }
}