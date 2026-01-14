#pragma once

#include <vector>
#include <list>
#include <string>
#include <memory>
#include <KamataEngine.h>
#include "Enemy.h"
#include "Player.h"
#include "DeathParticle.h"
#include "ExpOrb.h"
#include "HitParticle.h"
#include "../GameData.h"

class Player;
class Enemy;

class EnemyManager {
public:
    void Initialize(const std::string& csvPath, Player* player);
    void Update();
    void Draw(KamataEngine::Camera* camera);

    const std::vector<std::unique_ptr<Enemy>>& GetEnemies() const { return enemies_; }

    void SpawnEnemiesFromCSV(const std::string& filePath);

    // ★ 当たり判定をここに集約
    void CheckCollisions(Player* player);

    // ★ ヒットパーティクル描画
    void DrawHitParticles(KamataEngine::Camera* camera);

private:
    std::vector<std::unique_ptr<Enemy>> enemies_;
    Player* player_ = nullptr;

    std::list<std::unique_ptr<DeathParticle>> deathParticles_;
    std::list<std::unique_ptr<ExpOrb>> expOrbs_;

    // ★ GameScene から移動
    std::list<std::unique_ptr<HitParticle>> hitParticles_;

    KamataEngine::Audio* audio_ = nullptr;
    uint32_t hitSEHandle_ = 0;
};