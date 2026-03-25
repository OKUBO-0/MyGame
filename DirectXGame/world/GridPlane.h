#pragma once
#include <KamataEngine.h>
#include <memory>

/// <summary>
/// クラス名: GridPlane
/// 目的: グリッド床を表示する。
/// 責務: ワールド変換とモデルを保持し、初期化・更新・描画を行う。
/// </summary>
class GridPlane {
public:
    /// <summary>
    /// 初期化処理
    /// 目的: モデルやワールド変換の準備を行う。
    /// 引数: なし
    /// 戻り値: なし
    /// </summary>
    void Initialize();

    /// <summary>
    /// 毎フレーム更新処理
    /// 目的: 床の状態を更新する。
    /// 引数: なし
    /// 戻り値: なし
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// 目的: カメラに基づいて床を描画する。
    /// 引数: camera - 描画に使用するカメラ
    /// 戻り値: なし
    /// </summary>
    void Draw(KamataEngine::Camera* camera);

private:
    KamataEngine::WorldTransform worldTransform_; ///< 床の位置・回転・スケールを保持するワールド変換
    std::unique_ptr<KamataEngine::Model> planeModel_; ///< 床のモデル（グリッド模様付き）

    static constexpr float kGroundScale = 1000.0f; ///< 床のスケール（X,Z方向に広げる大きさ）
    static constexpr float kTileSize = 5.0f;       ///< 1タイルの大きさ（UV繰り返し数算出用）
};