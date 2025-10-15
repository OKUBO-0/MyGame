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
    void Draw();

    // 敵リスト取得
    const std::vector<Enemy*>& GetEnemies() const { return enemies_; }

private:
    // 敵リスト
    std::vector<Enemy*> enemies_;

    // 敵配置読み込み
    std::vector<KamataEngine::Vector3> LoadEnemyPositionsFromCSV(const std::string& filePath);

    // プレイヤー参照
    Player* player_ = nullptr;
};