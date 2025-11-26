#include "EnemyManager.h"
#include <fstream>
#include <sstream>
#include <cmath>

using namespace KamataEngine;

EnemyManager::EnemyManager() {}

EnemyManager::~EnemyManager() {
    // 管理している敵インスタンスをすべて解放
    for (auto enemy : enemies_) {
        delete enemy;
    }
    enemies_.clear();
}

void EnemyManager::Initialize(const std::string& csvPath, Player* player) {
    // プレイヤー参照を保持し、CSVから敵を生成
    player_ = player;
    SpawnEnemiesFromCSV(csvPath);
}

void EnemyManager::SpawnEnemiesFromCSV(const std::string& filePath) {
    // CSVファイルを開く（敵の種類・数・HP・EXP・配置距離を定義）
    std::ifstream file(filePath);
    if (!file.is_open()) {
        OutputDebugStringA(("CSV読み込み失敗: " + filePath + "\n").c_str());
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        int type = 0, count = 0, hp = 3, exp = 0;
        float distance = 0.0f;

        // CSVの各列を読み込み（type, distance, count, hp, exp）
        std::getline(ss, value, ','); type = std::stoi(value);
        std::getline(ss, value, ','); distance = std::stof(value);
        std::getline(ss, value, ','); count = std::stoi(value);
        std::getline(ss, value, ','); hp = std::stoi(value);
        std::getline(ss, value, ','); exp = std::stoi(value);

        // 指定された数だけ敵を円形に配置
        for (int i = 0; i < count; ++i) {
            float angle = (2.0f * 3.14159265f * i) / count; // 円周上の角度
            Vector3 pos = {
                player_->GetWorldPosition().x + std::cos(angle) * distance,
                0.0f,
                player_->GetWorldPosition().z + std::sin(angle) * distance
            };

            // 敵インスタンス生成と初期化
            Enemy* enemy = new Enemy();
            enemy->SetHP(hp);
            enemy->SetEXP(exp);
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
    // 各敵の更新処理（アクティブな敵のみ）
    for (auto enemy : enemies_) {
        if (enemy->IsActive()) {
            enemy->Update();
        }
    }

    // 敵同士の衝突判定と分離処理
    // 一定距離未満で重なった場合、押し返して距離を保つ
    const float minDist = 3.0f;       // 最低限保つべき距離
    const float pushStrength = 1.0f;  // 押し返しの強さ

    for (size_t i = 0; i < enemies_.size(); ++i) {
        Enemy* a = enemies_[i];
        if (!a->IsActive()) continue;

        for (size_t j = i + 1; j < enemies_.size(); ++j) {
            Enemy* b = enemies_[j];
            if (!b->IsActive()) continue;

            Vector3 posA = a->GetPosition();
            Vector3 posB = b->GetPosition();

            float dx = posB.x - posA.x;
            float dz = posB.z - posA.z;
            float distSq = dx * dx + dz * dz;

            // 距離が近すぎる場合に分離処理を行う
            if (distSq < minDist * minDist && distSq > 0.0001f) {
                float dist = std::sqrt(distSq);
                float overlap = minDist - dist;

                // 正規化ベクトル（押し返す方向）
                float nx = dx / dist;
                float nz = dz / dist;

                // 双方を押し返して距離を確保
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
    // アクティブな敵のみ描画
    for (auto enemy : enemies_) {
        if (enemy->IsActive()) {
            enemy->Draw(camera);
        }
    }
}