#include "IScene.h"

SCENE IScene::sceneNo = SCENE::Title;

IScene::~IScene() {}

SCENE IScene::GetSceneNo() const { return sceneNo; }