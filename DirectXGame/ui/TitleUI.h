#pragma once
#include <KamataEngine.h>

/// <summary>
/// タイトル画面で使用する3Dモデル演出を管理するクラス。
/// モデルの初期化、更新、描画、位置や回転制御を行う。
/// </summary>
class TitleUI {
public:
    /// <summary>
    /// コンストラクタ
    /// TitleUIの初期値を設定する
    /// </summary>
    TitleUI();

    /// <summary>
    /// デストラクタ
    /// 使用したリソースを解放する
    /// </summary>
    ~TitleUI();

    /// <summary>
    /// 初期化処理
    /// モデルやカメラの準備を行う
    /// </summary>
    void Initialize();

    /// <summary>
    /// 毎フレーム更新処理
    /// モデルの位置や回転などを更新する
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// モデルをカメラに基づいて画面に描画する
    /// </summary>
    void Draw();

    /// <summary>
    /// 終了処理
    /// リソース解放や後処理を行う
    /// </summary>
    void Finalize();

    /// <summary>
    /// モデルのZ座標を設定する
    /// </summary>
    /// <param name="z">設定するZ座標</param>
    void SetPositionZ(float z);

    /// <summary>
    /// モデルの現在のZ座標を取得する
    /// </summary>
    /// <returns>Z座標値</returns>
    float GetPositionZ() const;

    /// <summary>
    /// モデルに回転を加える
    /// </summary>
    /// <param name="angle">加える回転角度（ラジアン）</param>
    void AddRotation(float angle);

private:
    KamataEngine::WorldTransform worldTransform_; ///< モデルのワールドトランスフォーム
    KamataEngine::Camera camera_;                 ///< モデル描画用カメラ
    KamataEngine::Model* playerModel_ = nullptr;  ///< タイトル画面用モデル
};