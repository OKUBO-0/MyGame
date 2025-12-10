#pragma once

#include <KamataEngine.h>
#include <vector>
#include <memory>
#include <cstdint>
#include "Bullet.h"
#include "EnemyManager.h"
#include "RippleEffect.h"

class EnemyManager;

/// <summary>
/// プレイヤーキャラクターを表すクラス。
/// 入力処理、弾の発射、HPやEXPの管理、敵との連携を行う。
/// </summary>
class Player {
public:
    /// <summary>初期化処理（モデルや入力、弾管理の初期設定を行う）</summary>
    void Initialize();

    /// <summary>毎フレーム更新処理（入力に基づく移動や弾発射、状態更新を行う）</summary>
    void Update();

    /// <summary>描画処理（プレイヤーモデルや弾を画面に描画する）</summary>
    void Draw();

    /// <summary>プレイヤーのワールド座標を取得する</summary>
    KamataEngine::Vector3 GetWorldPosition() const { return worldTransform_.translation_; }

    /// <summary>プレイヤーが保持する弾リストを取得する</summary>
    const std::vector<std::unique_ptr<Bullet>>& GetBullets() const { return bullets_; }

    /// <summary>プレイヤーのカメラを取得する</summary>
    KamataEngine::Camera& GetCamera() { return camera_; }

    /// <summary>敵管理クラスを関連付ける</summary>
    void SetEnemyManager(EnemyManager* manager) { enemyManager_ = manager; }

    /// <summary>ダメージを受けてHPを減少させる（無敵状態の場合は無効）</summary>
    void TakeDamage();

    /// <summary>プレイヤーが無敵状態かどうかを判定する</summary>
    bool IsInvincible() const { return invincible_; }

    /// <summary>プレイヤーが死亡状態かどうかを判定する</summary>
    bool IsDead() const { return lifeStock_ <= 0; }

    /// <summary>経験値を加算する</summary>
    void AddEXP(int32_t amount);

    /// <summary>経験値、レベル関連のゲッター </summary>
    int32_t GetEXP() const { return exp_; }

    /// <summary>総獲得経験値を取得する </summary>
    int32_t GetTotalEXP() const { return totalExp_; }

    /// <summary>現在のレベルを取得する </summary>
    int32_t GetLevel() const { return level_; }

    /// <summary>次のレベルに必要な経験値を取得する </summary>
    int32_t GetNextLevelEXP() const { return nextLevelExp_; }

    /// <summary>レベルアップが要求されているかを判定する </summary>
    bool IsLevelUpRequested() const { return levelUpRequested_; }

    /// <summary>レベルアップ要求フラグをクリアする </summary>
    void ClearLevelUpRequest() { levelUpRequested_ = false; }

    /// <summary>弾の攻撃力をアップグレードする </summary>
    void UpgradeBulletPower();

    /// <summary>弾の発射間隔をアップグレードする </summary>
    void UpgradeBulletCooldown();

    /// <summary>HPを回復する </summary>
    void RecoverHP();

    /// <summary>弾の攻撃力を取得する </summary>
    int32_t GetBulletPower() const { return bulletPower_; }

    /// <summary>現在のHPと最大HPを取得する </summary>
    int32_t GetHP() const { return lifeStock_; }

    /// <summary>最大HPを取得する </summary>
    int32_t GetMaxHP() const { return maxLifeStock_; }

private:
    KamataEngine::Input* input_ = nullptr;        ///< 入力管理
    KamataEngine::WorldTransform worldTransform_; ///< プレイヤーのワールドトランスフォーム
    KamataEngine::Camera camera_;                 ///< プレイヤー用カメラ
    std::unique_ptr<KamataEngine::Model> playerModel_; ///< プレイヤーモデル

    std::vector<std::unique_ptr<Bullet>> bullets_; ///< 弾リスト（スマートポインタで管理）
    float bulletCooldown_ = 1.0f;  ///< 弾発射間隔
    float bulletTimer_ = 0.0f;     ///< 弾発射タイマー
    float range_ = 30.0f;          ///< 弾の射程
    int32_t bulletPower_ = 1;      ///< 弾の攻撃力

    EnemyManager* enemyManager_ = nullptr; ///< 敵管理クラス参照

    int32_t lifeStock_ = 3;        ///< 現在HP
    int32_t maxLifeStock_ = 3;     ///< 最大HP
    bool invincible_ = false;      ///< 無敵状態フラグ
    float invincibleTimer_ = 0.0f; ///< 無敵時間管理
    bool visible_ = true;          ///< 描画可否フラグ

    int32_t exp_ = 0;              ///< 経験値
    int32_t totalExp_ = 0;         ///< 総獲得経験値（リセットしない）
    int32_t level_ = 1;            ///< 現在レベル
    int32_t nextLevelExp_ = 1;     ///< 次のレベルに必要な経験値
    bool levelUpRequested_ = false; ///< レベルアップ要求フラグ

    std::vector<std::unique_ptr<RippleEffect>> effects_; ///< パーティクルリスト
    float effectTimer_ = 0.0f;
    static constexpr float kEffectInterval = 0.2f; ///< パーティクル生成間隔
};