#pragma once
#include <KamataEngine.h>

/// <summary>
/// ヒット時に表示される火花パーティクルを管理するクラス。
/// 寿命や拡散方向を持ち、一定時間で消滅する。
/// </summary>
class HitParticle {
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
    /// パーティクルが有効かどうかを判定する
    /// </summary>
    /// <returns>true: 有効 / false: 無効</returns>
    bool IsActive() const { return active_; }

private:
    KamataEngine::WorldTransform worldTransform_; ///< ワールド変換
    KamataEngine::Model* model_ = nullptr;        ///< パーティクルモデル

    static constexpr float kLifetime = 0.3f;      ///< 火花の寿命（短め）
    float age_ = 0.0f;                            ///< 経過時間
    float alpha_ = 1.0f;                          ///< 透明度
    bool active_ = true;                          ///< 有効フラグ

    KamataEngine::Vector3 velocity_;              ///< 拡散方向の速度
};