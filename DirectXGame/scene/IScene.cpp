#include "IScene.h"

/// <summary>
/// 静的メンバ変数の初期化
/// 現在のシーン番号を保持する変数で、初期値はタイトルシーン
/// </summary>
Scene IScene::sceneNo_ = Scene::Title;

IScene::~IScene() {
    /// <summary>
    /// 基底クラスのデストラクタ（派生クラスで必要に応じてリソース解放）
    /// </summary>
}

// <summary>
// 現在のシーン番号を返す
// この値を参照することで、シーン遷移の判定や処理分岐を行う
// </summary>
Scene IScene::GetSceneNo() const {
    return sceneNo_;
}