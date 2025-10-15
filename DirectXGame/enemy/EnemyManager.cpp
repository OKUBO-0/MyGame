#include "EnemyManager.h"
#include <fstream>
#include <sstream>
#include <KamataEngine.h>

using namespace KamataEngine;

EnemyManager::EnemyManager() {}

EnemyManager::~EnemyManager() {
    for (auto enemy : enemies_) {
        delete enemy;
    }
    enemies_.clear();
}

void EnemyManager::Initialize(const std::string& csvPath, Player* player) {
    player_ = player;

    // CSVから敵座標を読み込み
    const auto enemyPositions = LoadEnemyPositionsFromCSV(csvPath);

    // 敵を生成・配置
    for (const auto& pos : enemyPositions) {
        Enemy* enemy = new Enemy();
        enemy->Initialize();
        enemy->SetPlayer(player_);
        enemy->SetPosition(pos);
        enemies_.push_back(enemy);
    }
}

void EnemyManager::Update() {
    for (auto enemy : enemies_) {
        enemy->Update();
    }
}

void EnemyManager::Draw() {
    for (auto enemy : enemies_) {
        enemy->Draw();
    }
}

std::vector<Vector3> EnemyManager::LoadEnemyPositionsFromCSV(const std::string& filePath) {
    std::vector<Vector3> positions;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        OutputDebugStringA(("CSV読み込み失敗: " + filePath + "\n").c_str());
        return positions;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        float x = 0.0f, y = 0.0f, z = 0.0f;

        std::getline(ss, value, ',');
        x = std::stof(value);
        std::getline(ss, value, ',');
        y = std::stof(value);
        std::getline(ss, value, ',');
        z = std::stof(value);

        positions.emplace_back(x, y, z);
    }

    file.close();
    return positions;
}