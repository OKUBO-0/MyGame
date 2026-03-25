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
#include "../GameData.h"

class Player;
class PlayerManager;

class EnemyManager {
public:
    void Initialize(const std::string& csvPath, Player* player, PlayerManager* playerManager);
    void Update();
    void Draw(KamataEngine::Camera* camera);

    const std::vector<std::unique_ptr<Enemy>>& GetEnemies() const { return enemies_; }

    // 敵タイプ定義を CSV から読み込む
    void LoadEnemyTypes(const std::string& filePath);

    // 当たり判定
    void CheckCollisions(Player* player, PlayerManager* playerManager);

    // ヒットパーティクル描画
    void DrawHitParticles(KamataEngine::Camera* camera);

    const std::list<std::unique_ptr<ExpOrb>>& GetExpOrbs() const { return expOrbs_; }

private:
    static constexpr size_t kMaxActiveEnemies = 128;
    static constexpr size_t kMaxDeathParticles = 256;
    static constexpr size_t kMaxHitParticles = 256;

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

private:
    std::vector<std::unique_ptr<Enemy>> enemies_;
    Player* player_ = nullptr;
    PlayerManager* playerManager_ = nullptr;

    std::list<std::unique_ptr<DeathParticle>> deathParticles_;
    std::list<std::unique_ptr<ExpOrb>> expOrbs_;
    std::list<std::unique_ptr<HitParticle>> hitParticles_;

    KamataEngine::Audio* audio_ = nullptr;
    uint32_t hitSEHandle_ = 0;

    // 敵タイプ定義
    std::vector<EnemyTypeData> enemyTypes_;

    // 無限湧き用
    float elapsedTime_ = 0.0f;
    float spawnTimer_ = 0.0f;
    float spawnInterval_ = 2.0f;
};
