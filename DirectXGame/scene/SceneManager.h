#pragma once
#include <memory>
#include <unordered_map>
#include <functional>
#include "IScene.h"

class SceneManager {
public:
    SceneManager();
    ~SceneManager();

    void Update();
    void Draw();

    void RegisterScene(SCENE scene, std::function<std::unique_ptr<IScene>()> createFunc);
    void ChangeScene(SCENE scene);

private:
    std::unordered_map<SCENE, std::function<std::unique_ptr<IScene>()>> sceneFactory_;
    std::unique_ptr<IScene> currentScene_;
    SCENE currentSceneNo_;
};