#pragma once

#include <vector>
#include <list>
#include <string>
#include <memory>
#include <KamataEngine.h>
#include "Enemy.h"
#include "DeathParticle.h"
#include "ExpOrb.h"
#include "HitParticle.h"

namespace DirectXGame {

class Player;
class PlayerManager;

/// <summary>
/// 敵全体の生成と戦闘処理を管理するクラス。
/// 敵の湧き、更新、当たり判定、ドロップ、パーティクルをまとめて制御する。
/// </summary>
class EnemyManager {
public:
    /// <summary>
    /// 初期化処理
    /// 目的: 敵定義、参照先、効果音を読み込んで管理状態を整える。
    /// 引数: csvPath - 敵定義CSVのパス
    /// 引数: player - 追跡対象プレイヤー
    /// 引数: playerManager - ダメージやEXP付与に使うプレイヤー管理
    /// 戻り値: なし
    /// </summary>
    void Initialize(const std::string& csvPath, Player* player, PlayerManager* playerManager);

    /// <summary>
    /// 更新処理
    /// 目的: 敵の生成、更新、再配置、演出更新を毎フレーム進める。
    /// 引数: なし
    /// 戻り値: なし
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// 目的: 敵本体、EXPオーブ、死亡パーティクルを描画する。
    /// 引数: camera - 描画に使用するカメラ
    /// 戻り値: なし
    /// </summary>
    void Draw(KamataEngine::Camera* camera);

    const std::vector<std::unique_ptr<Enemy>>& GetEnemies() const { return enemies_; }

    /// <summary>
    /// 敵定義読み込み
    /// 目的: CSV から敵の基礎ステータスと出現数を取得する。
    /// 引数: filePath - 読み込むCSVファイルパス
    /// 戻り値: なし
    /// </summary>
    void LoadEnemyTypes(const std::string& filePath);

    /// <summary>
    /// 当たり判定処理
    /// 目的: 弾、敵、プレイヤーの衝突を判定してダメージ処理を行う。
    /// 引数: player - 接触判定を行うプレイヤー
    /// 引数: playerManager - 攻撃力や被ダメージ処理を扱う管理クラス
    /// 戻り値: なし
    /// </summary>
    void CheckCollisions(Player* player, PlayerManager* playerManager);

    /// <summary>
    /// ヒットパーティクル描画
    /// 目的: ヒット時に発生した火花演出だけを別パスで描画する。
    /// 引数: camera - 描画に使用するカメラ
    /// 戻り値: なし
    /// </summary>
    void DrawHitParticles(KamataEngine::Camera* camera);

    const std::list<std::unique_ptr<ExpOrb>>& GetExpOrbs() const { return expOrbs_; }
    int32_t GetTotalKillCount() const { return totalKillCount_; }

private:
    static constexpr size_t kMaxActiveEnemies = 128;
    static constexpr size_t kMaxDeathParticles = 256;
    static constexpr size_t kMaxHitParticles = 256;
    static constexpr float kDeltaTime = 1.0f / 60.0f;
    static constexpr float kSpawnUnlockInterval = 20.0f;
    static constexpr float kSpawnDistance = 50.0f;
    static constexpr float kRespawnDistance = 75.0f;
    static constexpr float kRespawnRadius = 60.0f;
    static constexpr float kMinSpawnInterval = 0.5f;
    static constexpr float kBaseSpawnInterval = 2.0f;
    static constexpr float kSpawnAcceleration = 0.01f;
    static constexpr float kEnemySeparationDistance = 3.0f;
    static constexpr float kEnemySeparationStrength = 1.0f;
    static constexpr float kNormalBulletHitDistanceSq = 4.0f;
    static constexpr float kOrbitBulletHitDistanceSq = 25.0f;
    static constexpr float kPlayerContactDistance = 3.0f;
    static constexpr float kPlayerContactDistanceSq = kPlayerContactDistance * kPlayerContactDistance;
    static constexpr int32_t kDeathParticleSpawnCount = 5;
    static constexpr int32_t kHitParticleSpawnCount = 4;

    struct EnemyTypeData {
        int32_t type;
        int32_t baseHP;
        float   baseSpeed;
        int32_t baseEXP;
        int32_t spawnCount;
    };

    void SpawnEnemies();                     // 無限湧き
    void SpawnOneEnemy(const EnemyTypeData& data);
    size_t GetActiveEnemyCount() const;
    void UpdateSpawnState();
    void UpdateEnemies();
    void RemoveInactiveEnemies();
    void RelocateFarEnemies();
    void UpdateEffects();
    void ResolveEnemySeparation();
    void SpawnDeathEffects(const Enemy& enemy);
    void SpawnHitParticles(const KamataEngine::Vector3& position);
    bool TryHandleBulletHit(Enemy& enemy, const KamataEngine::Vector3& impactPosition,
                            int32_t damage, float knockStrength);
    void CheckNormalBulletCollisions(PlayerManager& playerManager);
    void CheckOrbitBulletCollisions(PlayerManager& playerManager);
    void CheckDroneBulletCollisions(PlayerManager& playerManager);
    void CheckPlayerCollisions(Player& player, PlayerManager& playerManager);

private:
    std::vector<std::unique_ptr<Enemy>> enemies_;
    Player* player_ = nullptr;
    PlayerManager* playerManager_ = nullptr;

    std::list<std::unique_ptr<DeathParticle>> deathParticles_;
    std::list<std::unique_ptr<ExpOrb>> expOrbs_;
    std::list<std::unique_ptr<HitParticle>> hitParticles_;

    KamataEngine::Audio* audio_ = nullptr;
    uint32_t hitSEHandle_ = 0;
    uint32_t playerDamageSEHandle_ = 0;

    // 敵タイプ定義
    std::vector<EnemyTypeData> enemyTypes_;

    // 無限湧き用
    float elapsedTime_ = 0.0f;
    float spawnTimer_ = 0.0f;
    float spawnInterval_ = kBaseSpawnInterval;
    int32_t totalKillCount_ = 0;
};

} // namespace DirectXGame
