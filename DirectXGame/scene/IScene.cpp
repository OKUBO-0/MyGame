#include "IScene.h"

namespace EngineLayer {

constexpr Scene kInitialScene = Scene::Title;

} // namespace EngineLayer

/// <summary>
/// 静的メンバ変数の初期化
/// 現在のシーン番号を保持する変数で、初期値はタイトルシーン
/// </summary>
Scene IScene::sceneNo_ = EngineLayer::kInitialScene;

/// <summary>
/// 基底クラスのデストラクタ
/// 派生クラスで必要に応じてリソース解放を行う
/// </summary>
IScene::~IScene() = default;

/// <summary>
/// 現在のシーン番号を返す
/// この値を参照することで、シーン遷移の判定や処理分岐を行う
/// </summary>
Scene IScene::GetSceneNo() const {
    return sceneNo_;
}
