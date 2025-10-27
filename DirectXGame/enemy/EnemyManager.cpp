#include "EnemyManager.h"
#include <fstream>
#include <sstream>
#include <cmath>

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
    SpawnEnemiesFromCSV(csvPath);
}

void EnemyManager::SpawnEnemiesFromCSV(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        OutputDebugStringA(("CSV読み込み失敗: " + filePath + "\n").c_str());
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        int type = 0, count = 0;
        float distance = 0.0f;

        std::getline(ss, value, ',');
        type = std::stoi(value);
        std::getline(ss, value, ',');
        distance = std::stof(value);
        std::getline(ss, value, ',');
        count = std::stoi(value);

        for (int i = 0; i < count; ++i) {
            float angle = (2.0f * 3.14159265f * i) / count;
            Vector3 pos = {
                player_->GetWorldPosition().x + std::cos(angle) * distance,
                0.0f,
                player_->GetWorldPosition().z + std::sin(angle) * distance
            };

            Enemy* enemy = new Enemy();
            enemy->Initialize();
            enemy->SetPlayer(player_);
            enemy->SetPosition(pos);
            enemy->SetModelByType(type);
            enemies_.push_back(enemy);
        }
    }

    file.close();
}

void EnemyManager::Update() {
    // 敵の更新
    for (auto enemy : enemies_) {
        if (enemy->IsActive()) {
            enemy->Update();
        }
    }

    // 敵同士の衝突判定（簡易分離）
    const float minDist = 2.0f;
    const float pushStrength = 0.05f;

    for (size_t i = 0; i < enemies_.size(); ++i) {
        Enemy* a = enemies_[i];
        if (!a->IsActive()) continue;

        for (size_t j = i + 1; j < enemies_.size(); ++j) {
            Enemy* b = enemies_[j];
            if (!b->IsActive()) continue;

            KamataEngine::Vector3 posA = a->GetPosition();
            KamataEngine::Vector3 posB = b->GetPosition();

            float dx = posB.x - posA.x;
            float dz = posB.z - posA.z;
            float distSq = dx * dx + dz * dz;

            if (distSq < minDist * minDist && distSq > 0.0001f) {
                float dist = std::sqrt(distSq);
                float overlap = minDist - dist;

                // 正規化ベクトル
                float nx = dx / dist;
                float nz = dz / dist;

                // 押し返し
                posA.x -= nx * overlap * pushStrength;
                posA.z -= nz * overlap * pushStrength;
                posB.x += nx * overlap * pushStrength;
                posB.z += nz * overlap * pushStrength;

                a->SetPosition(posA);
                b->SetPosition(posB);
            }
        }
    }
}

void EnemyManager::Draw(Camera* camera) {
    for (auto enemy : enemies_) {
        if (enemy->IsActive()) {
            enemy->Draw(camera);
        }
    }
}