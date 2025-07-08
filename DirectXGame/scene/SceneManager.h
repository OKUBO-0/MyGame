#pragma once
#include <memory>
#include <unordered_map>
#include <functional>
#include "IScene.h"

enum class SceneName {
	Title,
	Game,
};

class SceneManager {
public:
	SceneManager();
	~SceneManager();

	void Update();
	void Draw();

	// シーン登録
	void RegisterScene(SceneName name, std::function<std::unique_ptr<IScene>()> createFunc);

	// 初期シーンの設定
	void ChangeScene(SceneName name);

private:
	std::unordered_map<SceneName, std::function<std::unique_ptr<IScene>()>> sceneFactory_;
	std::unique_ptr<IScene> currentScene_;
	SceneName currentSceneName_;
};