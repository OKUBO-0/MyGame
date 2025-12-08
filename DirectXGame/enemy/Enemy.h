#pragma once

#include <KamataEngine.h>
#include "Player.h"

class Player;

/// <summary>
/// 敵キャラクターを表すクラス。
/// モデルや位置、HP、EXPなどの状態を管理し、プレイヤーとの連携や描画を行う。
/// </summary>
class Enemy {
public:
    /// <summary>
    /// コンストラクタ
    /// 敵の初期値を設定する
    /// </summary>
    Enemy();

    /// <summary>
    /// デストラクタ
    /// 使用したリソースを解放する
    /// </summary>
    ~Enemy();

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

    /// <summary>
    /// 描画処理
    /// 敵モデルをカメラに基づいて描画する
    /// </summary>
    /// <param name="camera">描画に使用するカメラ</param>
    void Draw(KamataEngine::Camera* camera);

    /// <summary>
    /// 敵の位置を設定する
    /// </summary>
    /// <param name="pos">設定する座標</param>
    void SetPosition(const KamataEngine::Vector3& pos) { worldTransform_.translation_ = pos; }

    /// <summary>
    /// プレイヤー情報を関連付ける
    /// </summary>
    /// <param name="player">関連付けるプレイヤー</param>
    void SetPlayer(Player* player) { player_ = player; }

    /// <summary>
    /// 敵の種類に応じてモデルを設定する
    /// </summary>
    /// <param name="type">敵の種類を表す整数値</param>
    void SetModelByType(int32_t type);

    /// <summary>
    /// 敵の現在位置を取得する
    /// </summary>
    /// <returns>敵の座標</returns>
    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

    /// <summary>
    /// 敵がアクティブかどうかを判定する
    /// </summary>
    /// <returns>true: アクティブ / false: 非アクティブ</returns>
    bool IsActive() const { return active_; }

    /// <summary>
    /// 敵を非アクティブ状態にする
    /// </summary>
    void Deactivate() { active_ = false; }

    /// <summary>
    /// 敵のHPを設定する
    /// </summary>
    /// <param name="hp">設定するHP値</param>
    void SetHP(int32_t hp) { hp_ = hp; }

    /// <summary>
    /// 敵のHPを取得する
    /// </summary>
    /// <returns>現在のHP値</returns>
    int32_t GetHP() const { return hp_; }

    /// <summary>
    /// ダメージを受けてHPを減少させる
    ///（オプションでノックバック方向と強さを指定可能）
    /// </summary>
    /// <param name="damage">受けるダメージ量</param>
    void TakeDamage(int32_t damage, const KamataEngine::Vector3& knockDir = { 0.0f, 0.0f, 0.0f }, float strength = 0.0f);

    /// <summary>
    /// 敵の撃破時に得られる経験値を設定する
    /// </summary>
    /// <param name="exp">設定する経験値</param>
    void SetEXP(int32_t exp) { exp_ = exp; }

    /// <summary>
    /// 敵の撃破時に得られる経験値を取得する
    /// </summary>
    /// <returns>経験値の値</returns>
    int32_t GetEXP() const { return exp_; }

    /// 死亡直後かどうかを判定
    bool JustDied() const { return justDied_; }
    void ResetJustDied() { justDied_ = false; }

private:
    KamataEngine::WorldTransform worldTransform_; ///< 敵のワールドトランスフォーム
    KamataEngine::Model* enemyModel_ = nullptr;   ///< 敵モデル

    float speed_ = 0.1f; ///< 移動速度

    int32_t hp_ = 3;     ///< HP値
    int32_t exp_ = 0;    ///< 経験値
    bool active_ = true; ///< アクティブ状態フラグ

    Player* player_ = nullptr; ///< プレイヤー参照

    // ヒット時の白化
    KamataEngine::ObjectColor* objectColor_ = nullptr;
    float hitFlashTimer_ = 0.0f;
    static constexpr float kHitFlashDuration = 0.12f; ///< ヒット時の白化時間
    uint32_t whiteTextureHandle_ = 0;

    // ノックバック関連
    KamataEngine::Vector3 knockbackVelocity_ = { 0.0f, 0.0f, 0.0f };
    float knockbackTimer_ = 0.0f;
    static constexpr float kKnockbackDuration = 0.18f; ///< ノックバック継続時間

    bool justDied_ = false; ///< 死亡直後フラグ
};