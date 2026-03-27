#pragma once

#include <KamataEngine.h>
#include <vector>
#include <memory>
#include <cstdint>
#include <string>

#include "Player.h"
#include "../../effects/RippleEffect.h"
#include "../weapons/NormalBullet.h"
#include "../weapons/OrbitBullet.h"
#include "../weapons/Drone.h"

namespace DirectXGame {

class EnemyManager;

/// <summary>
/// プレイヤーの成長と戦闘リソースを管理するクラス。
/// HP、EXP、武器、ドローン、無敵時間などのプレイヤー状態を一元管理する。
/// </summary>
class PlayerManager {
public:
    /// <summary>
    /// 初期化処理
    /// 目的: 管理対象プレイヤーを関連付け、表示状態を同期する。
    /// 引数: player - 管理対象のプレイヤー本体
    /// 戻り値: なし
    /// </summary>
    void Initialize(Player* player);

    /// <summary>
    /// CSV 読み込み
    /// 目的: プレイヤー初期パラメータを外部ファイルから反映する。
    /// 引数: filePath - 読み込むCSVファイルパス
    /// 戻り値: なし
    /// </summary>
    void LoadStatusFromCSV(const std::string& filePath);

    /// <summary>
    /// 更新処理
    /// 目的: 無敵時間、弾、ドローン、エフェクトを毎フレーム更新する。
    /// 引数: なし
    /// 戻り値: なし
    /// </summary>
    void Update(float deltaTime);

    /// <summary>
    /// 描画処理
    /// 目的: プレイヤー付随の弾やエフェクトを描画する。
    /// 引数: camera - 描画に使用するカメラ
    /// 戻り値: なし
    /// </summary>
    void Draw(KamataEngine::Camera* camera);

    // HP管理
    /// <summary>
    /// ダメージ処理
    /// 目的: プレイヤーのHPを減少させ、無敵時間を開始する。
    /// 引数: なし
    /// 戻り値: なし
    /// </summary>
    void TakeDamage();

    /// <summary>
    /// HP回復処理
    /// 目的: HPを1だけ回復し、最大値を超えないよう制限する。
    /// 引数: なし
    /// 戻り値: なし
    /// </summary>
    void RecoverHP();
    int32_t GetHP() const { return lifeStock_; }
    int32_t GetMaxHP() const { return maxLifeStock_; }
    bool IsInvincible() const { return invincible_; }
    bool IsDead() const { return lifeStock_ <= 0; }

    // EXP / レベル
    /// <summary>
    /// 経験値加算
    /// 目的: EXP を増加させ、必要ならレベルアップ要求を立てる。
    /// 引数: amount - 加算する経験値量
    /// 戻り値: なし
    /// </summary>
    void AddEXP(int32_t amount);
    int32_t GetEXP() const { return exp_; }
    int32_t GetTotalEXP() const { return totalExp_; }
    int32_t GetLevel() const { return level_; }
    int32_t GetNextLevelEXP() const { return nextLevelExp_; }
    bool IsLevelUpRequested() const { return levelUpRequested_; }
    void ClearLevelUpRequest() { levelUpRequested_ = false; }

    // 攻撃力
    int32_t GetAttackPower() const { return attackPower_; }
    void UpgradeAttackPower() { attackPower_++; }

    // 基礎ステータス強化
    void IncreaseMaxHP();
    void UpgradeMoveSpeed();
    int32_t GetMoveSpeedLevel() const { return moveSpeedLevel_; }

    // 通常弾
    /// <summary>
    /// 通常弾強化
    /// 目的: 発射間隔を短縮して通常弾性能を上げる。
    /// 引数: なし
    /// 戻り値: なし
    /// </summary>
    void UpgradeNormalBullets();
    const std::vector<std::unique_ptr<NormalBullet>>& GetNormalBullets() const { return normalBullets_; }
    float GetNormalBulletInterval() const { return normalBulletInterval_; }

    // 周囲弾
    /// <summary>
    /// 周囲弾追加
    /// 目的: 周囲弾を有効化し、初期弾を生成する。
    /// 引数: なし
    /// 戻り値: なし
    /// </summary>
    void AddOrbitBullets();

    /// <summary>
    /// 周囲弾強化
    /// 目的: 周囲弾の数を増やして攻撃範囲を拡張する。
    /// 引数: なし
    /// 戻り値: なし
    /// </summary>
    void UpgradeOrbitBullets();
    bool HasOrbitBullets() const { return hasOrbitBullets_; }
    const std::vector<std::unique_ptr<OrbitBullet>>& GetOrbitBullets() const { return orbitBullets_; }

    // ドローン
    /// <summary>
    /// ドローン追加
    /// 目的: ドローンを生成して自動攻撃を有効化する。
    /// 引数: なし
    /// 戻り値: なし
    /// </summary>
    void AddDrone();

    /// <summary>
    /// ドローン強化
    /// 目的: ドローンの攻撃間隔を短縮する。
    /// 引数: なし
    /// 戻り値: なし
    /// </summary>
    void UpgradeDrone();
    bool HasDrone() const { return hasDrone_; }
    const std::unique_ptr<Drone>& GetDrone() const { return drone_; }

    /// <summary>
    /// 敵マネージャ設定
    /// 目的: ドローンの索敵などに使う敵管理参照を保持する。
    /// 引数: manager - 関連付ける敵マネージャ
    /// 戻り値: なし
    /// </summary>
    void SetEnemyManager(EnemyManager* manager) { enemyManager_ = manager; }

private:
    Player* player_ = nullptr;
    EnemyManager* enemyManager_ = nullptr;

    // HP管理
    bool invincible_ = false;
    float invincibleTimer_ = 0.0f;
    bool visible_ = true;
    static constexpr float kInvincibilityDuration = 1.25f;

    // EXP / レベル
    int32_t level_ = 1;
    int32_t nextLevelExp_ = 10;
    int32_t maxLifeStock_ = 3;
    int32_t lifeStock_ = 3;
    int32_t exp_ = 0;
    int32_t totalExp_ = 0;
    int32_t attackPower_ = 1;
    int32_t moveSpeedLevel_ = 0;
    bool levelUpRequested_ = false;

    // エフェクト
    std::vector<std::unique_ptr<RippleEffect>> effects_;
    float effectTimer_ = 0.0f;
    static constexpr float kEffectInterval = 0.2f;

    // 通常弾
    std::vector<std::unique_ptr<NormalBullet>> normalBullets_;
    bool hasNormalBullets_ = true;
    float normalBulletInterval_ = 0.85f;
    float normalBulletTimer_ = 0.0f;

    static constexpr int32_t kMaxLifeStockCap = 6;
    static constexpr int32_t kMoveSpeedUpgradeCap = 5;
    static constexpr float kMoveSpeedUpgradeStep = 3.0f;
    static constexpr float kMoveSpeedMax = 45.0f;

    // 周囲弾
    std::vector<std::unique_ptr<OrbitBullet>> orbitBullets_;
    bool hasOrbitBullets_ = false;

    // ドローン
    std::unique_ptr<Drone> drone_;
    bool hasDrone_ = false;
    float droneInterval_ = 2.0f;
    float droneTimer_ = 0.0f;

    void UpdateInvincibility(float deltaTime);
    void UpdateNormalBullets(float deltaTime);
    void UpdateOrbitBullets(float deltaTime);
    void UpdateDrone(float deltaTime);
    void UpdateEffects(float deltaTime);
};

} // namespace DirectXGame
