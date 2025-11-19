#pragma once

#include <KamataEngine.h>
#include <vector>
#include "Bullet.h"
#include "EnemyManager.h"

class EnemyManager;

/// <summary>
/// プレイヤーキャラクターを表すクラス。
/// 入力処理、弾の発射、HPやEXPの管理、敵との連携を行う。
/// </summary>
class Player {
public:
    /// <summary>
    /// コンストラクタ
    /// プレイヤーの初期値を設定する
    /// </summary>
    Player();

    /// <summary>
    /// デストラクタ
    /// 使用したリソースを解放する
    /// </summary>
    ~Player();

    /// <summary>
    /// 初期化処理
    /// モデルや入力、弾管理の初期設定を行う
    /// </summary>
    void Initialize();

    /// <summary>
    /// 毎フレーム更新処理
    /// 入力に基づく移動や弾発射、状態更新を行う
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// プレイヤーモデルや弾を画面に描画する
    /// </summary>
    void Draw();

    /// <summary>
    /// プレイヤーのワールド座標を取得する
    /// </summary>
    /// <returns>プレイヤーの座標</returns>
    KamataEngine::Vector3 GetWorldPosition() const { return worldTransform_.translation_; }

    /// <summary>
    /// プレイヤーが保持する弾リストを取得する
    /// </summary>
    /// <returns>弾ポインタのベクター参照</returns>
    const std::vector<Bullet*>& GetBullets() const { return bullets_; }

    /// <summary>
    /// プレイヤーのカメラを取得する
    /// </summary>
    /// <returns>カメラ参照</returns>
    KamataEngine::Camera& GetCamera() { return camera_; }

    /// <summary>
    /// 敵管理クラスを関連付ける
    /// </summary>
    /// <param name="manager">関連付けるEnemyManager</param>
    void SetEnemyManager(EnemyManager* manager) { enemyManager_ = manager; }

    /// <summary>
    /// ダメージを受けてHPを減少させる
    /// 無敵状態の場合は無効
    /// </summary>
    void TakeDamage();

    /// <summary>
    /// プレイヤーが無敵状態かどうかを判定する
    /// </summary>
    /// <returns>true: 無敵 / false: 通常</returns>
    bool IsInvincible() const { return invincible_; }

    /// <summary>
    /// プレイヤーが死亡状態かどうかを判定する
    /// </summary>
    /// <returns>true: HPが0以下 / false: 生存</returns>
    bool IsDead() const { return lifeStock_ <= 0; }

    /// <summary>
    /// 経験値を加算する
    /// </summary>
    /// <param name="amount">加算する経験値</param>
    void AddEXP(int amount);

    /// <summary>
    /// 現在の経験値を取得する
    /// </summary>
    /// <returns>経験値の値</returns>
    int GetEXP() const { return exp_; }

    /// <summary>
    /// 現在のレベルを取得する
    /// </summary>
    /// <returns>レベル値</returns>
    int GetLevel() const { return level_; }

    /// <summary>
    /// 次のレベルに必要な経験値を取得する
    /// </summary>
    /// <returns>必要経験値</returns>
    int GetNextLevelEXP() const { return nextLevelExp_; }

    /// <summary>
    /// レベルアップ選択が要求されているか判定する
    /// </summary>
    /// <returns>true: 要求あり / false: 要求なし</returns>
    bool IsLevelUpRequested() const { return levelUpRequested_; }

    /// <summary>
    /// レベルアップ要求をクリアする
    /// </summary>
    void ClearLevelUpRequest() { levelUpRequested_ = false; }

    /// <summary>
    /// 弾の攻撃力を強化する
    /// </summary>
    void UpgradeBulletPower();

    /// <summary>
    /// 弾の発射間隔を短縮する
    /// </summary>
    void UpgradeBulletCooldown();

    /// <summary>
    /// HPを回復する
    /// </summary>
    void RecoverHP();

    /// <summary>
    /// 弾の攻撃力を取得する
    /// </summary>
    /// <returns>攻撃力の値</returns>
    int GetBulletPower() const { return bulletPower_; }

    /// <summary>
    /// 現在のHPを取得する
    /// </summary>
    /// <returns>HP値</returns>
    int GetHP() const { return lifeStock_; }

    /// <summary>
    /// 最大HPを取得する
    /// </summary>
    /// <returns>最大HP値</returns>
    int GetMaxHP() const { return maxLifeStock_; }

private:
    KamataEngine::Input* input_ = nullptr;        ///< 入力管理
    KamataEngine::WorldTransform worldTransform_; ///< プレイヤーのワールドトランスフォーム
    KamataEngine::Camera camera_;                 ///< プレイヤー用カメラ
    KamataEngine::Model* playerModel_ = nullptr;  ///< プレイヤーモデル

    std::vector<Bullet*> bullets_; ///< 弾リスト
    float bulletCooldown_ = 1.0f;  ///< 弾発射間隔
    float bulletTimer_ = 0.0f;     ///< 弾発射タイマー
    float range_ = 30.0f;          ///< 弾の射程
    int bulletPower_ = 1;          ///< 弾の攻撃力

    EnemyManager* enemyManager_ = nullptr; ///< 敵管理クラス参照

    int lifeStock_ = 3;            ///< 現在HP
    int maxLifeStock_ = 3;         ///< 最大HP
    bool invincible_ = false;      ///< 無敵状態フラグ
    float invincibleTimer_ = 0.0f; ///< 無敵時間管理
    bool visible_ = true;          ///< 描画可否フラグ

    int exp_ = 0;                   ///< 経験値
    int level_ = 1;                 ///< 現在レベル
    int nextLevelExp_ = 1;          ///< 次のレベルに必要な経験値
    bool levelUpRequested_ = false; ///< レベルアップ要求フラグ
};