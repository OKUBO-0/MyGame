#pragma once

#include <KamataEngine.h>
#include "Player.h"
#include <memory>

class Player;

/// <summary>
/// 敵キャラクターを表すクラス。
/// モデルや位置、HP、EXPなどの状態を管理し、プレイヤーとの連携や描画を行う。
/// </summary>
class Enemy {
public:
    /// <summary>
    /// 初期化処理
    /// モデルやトランスフォームの設定を行う
    /// </summary>
    void Initialize();

    /// <summary>
    /// 毎フレーム更新処理
    /// 敵の挙動や状態を更新する
    /// </summary>
    void Update();

    void UpdateType2();

    /// <summary>
    /// 描画処理
    /// 敵モデルをカメラに基づいて描画する
    /// </summary>
    void Draw(KamataEngine::Camera* camera);

    /// <summary>敵の位置を設定する</summary>
    void SetPosition(const KamataEngine::Vector3& pos);

    /// <summary>プレイヤー情報を関連付ける</summary>
    void SetPlayer(Player* player);

    /// <summary>敵の種類に応じてモデルを設定する</summary>
    void SetModelByType(int32_t type);

    /// <summary>敵の現在位置を取得する</summary>
    KamataEngine::Vector3 GetPosition() const;

    /// <summary>敵がアクティブかどうかを判定する</summary>
    bool IsActive() const;

    /// <summary>敵を非アクティブ状態にする</summary>
    void Deactivate();

    /// <summary>敵のHPを設定する</summary>
    void SetHP(int32_t hp);

    /// <summary>敵のHPを取得する</summary>
    int32_t GetHP() const;

    /// <summary>
    /// ダメージを受けてHPを減少させる
    /// （オプションでノックバック方向と強さを指定可能）
    /// </summary>
    void TakeDamage(int32_t damage, const KamataEngine::Vector3& knockDir = { 0.0f, 0.0f, 0.0f }, float strength = 0.0f);

    /// <summary>敵の撃破時に得られる経験値を設定する</summary>
    void SetEXP(int32_t exp);

    /// <summary>敵の撃破時に得られる経験値を取得する</summary>
    int32_t GetEXP() const;

    /// <summary>死亡直後かどうかを判定する</summary>
    bool JustDied() const;

    /// <summary>死亡直後フラグをリセットする</summary>
    void ResetJustDied();

private:
    KamataEngine::WorldTransform worldTransform_; ///< 敵の位置・回転・スケールを保持するワールドトランスフォーム
    std::unique_ptr<KamataEngine::Model> enemyModel_; ///< 敵モデル（スマートポインタで管理）

    float speed_ = 0.15f; ///< 移動速度

    int32_t hp_ = 3;     ///< HP値
    int32_t exp_ = 0;    ///< 経験値
    bool active_ = true; ///< アクティブ状態フラグ

    Player* player_ = nullptr; ///< プレイヤー参照

    // ヒット時の白化
    std::unique_ptr<KamataEngine::ObjectColor> objectColor_; ///< ヒット時の色管理
    float hitFlashTimer_ = 0.0f;
    static constexpr float kHitFlashDuration = 0.12f; ///< ヒット時の白化時間
    uint32_t whiteTextureHandle_ = 0;

    // ノックバック関連
    KamataEngine::Vector3 knockbackVelocity_ = { 0.0f, 0.0f, 0.0f };
    float knockbackTimer_ = 0.0f;
    static constexpr float kKnockbackDuration = 0.18f; ///< ノックバック継続時間

    bool justDied_ = false; ///< 死亡直後フラグ

    int enemyType_ = 0;
    float speedMultiplier_ = 1.0f; // type1 のとき 2.0f などにする
    float approachSpeed_ = 1.0f; // プレイヤーに近づく速度 
    float circleSpeed_ = 1.0f; // 円軌道の速度（横方向）

    KamataEngine::Audio* audio_ = nullptr;
    uint32_t deathSEHandle_ = 0;
};