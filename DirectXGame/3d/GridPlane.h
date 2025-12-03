#pragma once
#include <KamataEngine.h>

/// <summary>
/// グリッド床を表示するクラス。
/// ワールド変換とモデルを保持し、初期化・更新・描画を行う。
/// </summary>
class GridPlane {
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    GridPlane();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~GridPlane();

    /// <summary>
    /// 初期化処理
    /// モデルやワールド変換の準備を行う
    /// </summary>
    void Initialize();

    /// <summary>
    /// 毎フレーム更新処理
    /// 床の状態を更新する
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// カメラに基づいて床を描画する
    /// </summary>
    /// <param name="camera">描画に使用するカメラ</param>
    void Draw(KamataEngine::Camera* camera);

private:
    KamataEngine::WorldTransform worldTransform_; ///< ワールド変換
    KamataEngine::Model* planeModel_ = nullptr;   ///< 平面モデル
};