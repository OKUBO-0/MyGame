#pragma once

#include <vector>
#include <string>
#include <KamataEngine.h>
#include "Enemy.h"
#include "Player.h"

class Player;
class Enemy;

class EnemyManager {
public:
    EnemyManager();
    ~EnemyManager();

    // 初期化と更新
    void Initialize(const std::string& csvPath, Player* player);
    void Update();
    void Draw(KamataEngine::Camera* camera);

    // 敵リスト取得
    const std::vector<Enemy*>& GetEnemies() const { return enemies_; }

private:
    // 敵リスト
    std::vector<Enemy*> enemies_;

    // プレイヤー参照
    Player* player_ = nullptr;

    // 敵出現処理
    void SpawnEnemiesFromCSV(const std::string& filePath);
};