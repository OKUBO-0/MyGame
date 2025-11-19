#pragma once
#include "../GameData.h"
#include <cstdint>

/// <summary>
/// シーンの種類を表す列挙型。
/// Title: タイトルシーン
/// Game: ゲームプレイシーン
/// Result: リザルトシーン
/// </summary>
enum class SCENE {
    Title,
    Game,
    Result,
};

/// <summary>
/// シーンの基本インターフェースを定義する抽象クラス。
/// 各シーンはこのクラスを継承し、初期化・更新・描画・終了処理を実装する。
/// </summary>
class IScene {
protected:
    static SCENE sceneNo; ///< 現在のシーン番号

public:
    /// <summary>
    /// 初期化処理
    /// シーン開始時に必要な準備を行う
    /// </summary>
    virtual void Initialize() = 0;

    /// <summary>
    /// 毎フレーム更新処理
    /// シーン内の状態を更新する
    /// </summary>
    virtual void Update() = 0;

    /// <summary>
    /// 描画処理
    /// シーン内のオブジェクトを画面に描画する
    /// </summary>
    virtual void Draw() = 0;

    /// <summary>
    /// 終了処理
    /// シーン終了時に必要な後処理を行う
    /// </summary>
    virtual void Finalize() = 0;

    /// <summary>
    /// シーンが終了状態かどうかを判定する
    /// </summary>
    /// <returns>true: 終了 / false: 継続</returns>
    virtual bool IsFinished() const = 0;

    /// <summary>
    /// デストラクタ
    /// 派生クラスのリソース解放を保証する
    /// </summary>
    virtual ~IScene();

    /// <summary>
    /// 現在のシーン番号を取得する
    /// </summary>
    /// <returns>現在のシーン番号</returns>
    SCENE GetSceneNo() const;

    /// <summary>
    /// 次のシーン番号を設定する
    /// </summary>
    /// <param name="next">設定するシーン番号</param>
    void SetSceneNo(SCENE next) { sceneNo = next; }

    /// <summary>
    /// 静的関数でシーン番号を設定する
    /// </summary>
    /// <param name="next">設定するシーン番号</param>
    static void SetStaticSceneNo(SCENE next) { sceneNo = next; }

    /// <summary>
    /// 静的関数で現在のシーン番号を取得する
    /// </summary>
    /// <returns>現在のシーン番号</returns>
    static SCENE GetStaticSceneNo() { return sceneNo; }
};