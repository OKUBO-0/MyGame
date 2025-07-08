#include "SceneManager.h"

SceneManager::SceneManager() {}

SceneManager::~SceneManager() {}

void SceneManager::RegisterScene(SceneName name, std::function<std::unique_ptr<IScene>()> createFunc) {
	sceneFactory_[name] = createFunc;
}

void SceneManager::ChangeScene(SceneName name) {
	if (sceneFactory_.find(name) != sceneFactory_.end()) {
		currentScene_ = sceneFactory_[name]();
		currentScene_->Initialize();
		currentSceneName_ = name;
	}
}

void SceneManager::Update() {
	if (!currentScene_) return;

	currentScene_->Update();

	if (currentScene_->IsFinished()) {
		if (currentSceneName_ == SceneName::Title) {
			ChangeScene(SceneName::Game);
		}
		else if (currentSceneName_ == SceneName::Game) {
			ChangeScene(SceneName::Title);
		}
	}
}

void SceneManager::Draw() {
	if (currentScene_) {
		currentScene_->Draw();
	}
}