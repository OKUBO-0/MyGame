#pragma once

#include <KamataEngine.h>

/// <summary>
/// 天球モデルを用いて背景を描画するクラス。
/// シーン全体の空を表現し、カメラやワールドトランスフォームを管理する。
/// </summary>
class SkyDome
{
public:
    /// <summary>
    /// コンストラクタ
    /// SkyDomeの初期値を設定する
    /// </summary>
    SkyDome();

    /// <summary>
    /// デストラクタ
    /// 使用したリソースを解放する
    /// </summary>
    ~SkyDome();

    /// <summary>
    /// 初期化処理
    /// 天球モデルの読み込みやワールドトランスフォームの設定を行う
    /// </summary>
    void Initialize();

    /// <summary>
    /// 毎フレーム更新処理
    /// カメラやトランスフォームの更新を行う
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// 天球モデルを画面に描画する
    /// </summary>
    void Draw();

private:
    KamataEngine::WorldTransform worldTransform_; ///< 天球のワールドトランスフォーム
    KamataEngine::Camera camera_;                 ///< 天球描画用カメラ
    KamataEngine::Model* skyModel_ = nullptr;     ///< 天球モデル
};