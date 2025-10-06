#pragma once
#include "Enemy.h"
#include "Player.h"
#include <vector>
#include <string>

class Player;
class Enemy;
class EnemyManager {
public:
    EnemyManager();
    ~EnemyManager();

    void Initialize(const std::string& csvPath, Player* player);
    void Update();
    void Draw();

    const std::vector<Enemy*>& GetEnemies() const { return enemies_; }

private:
    std::vector<Enemy*> enemies_;
    std::vector<KamataEngine::Vector3> LoadEnemyPositionsFromCSV(const std::string& filePath);
    Player* player_ = nullptr;
};