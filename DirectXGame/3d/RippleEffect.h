#pragma once
#include <KamataEngine.h>

/// <summary>
/// 水面に広がる波紋エフェクトを管理するクラス。
/// 一定時間で消滅し、スケールと透明度が変化する。
/// </summary>
class RippleEffect {
public:
    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="pos">生成位置</param>
    void Initialize(const KamataEngine::Vector3& pos);

    /// <summary>
    /// 毎フレーム更新処理
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="camera">描画に使用するカメラ</param>
    void Draw(KamataEngine::Camera* camera);

    /// <summary>
    /// エフェクトが有効かどうかを判定する
    /// </summary>
    /// <returns>true: 有効 / false: 無効</returns>
    bool IsActive() const { return active_; }

private:
    KamataEngine::WorldTransform worldTransform_; ///< ワールド変換
    KamataEngine::Model* model_ = nullptr;        ///< 波紋モデル

    static constexpr float kLifetime = 1.0f;      ///< 総寿命（秒）
    float age_ = 0.0f;                            ///< 経過時間
    float alpha_ = 1.0f;                          ///< 透明度

    static constexpr float kStartScale = 0.5f;    ///< 初期サイズ
    static constexpr float kEndScale = 1.5f;      ///< 最大サイズ

    bool active_ = true;                          ///< 有効フラグ
};