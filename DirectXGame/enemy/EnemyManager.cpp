#include "EnemyManager.h"
#include <fstream>
#include <sstream>
#include <cmath>

using namespace KamataEngine;

void EnemyManager::Initialize(const std::string& csvPath, Player* player) {
    // プレイヤー参照を保持し、CSVから敵を生成
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
        int32_t type = 0;
        int32_t count = 0;
        int32_t hp = 3;
        int32_t exp = 0;
        float distance = 0.0f;

        // CSVの各列を読み込み（type, distance, count, hp, exp）
        std::getline(ss, value, ','); type = std::stoi(value);
        std::getline(ss, value, ','); distance = std::stof(value);
        std::getline(ss, value, ','); count = std::stoi(value);
        std::getline(ss, value, ','); hp = std::stoi(value);
        std::getline(ss, value, ','); exp = std::stoi(value);

        // 指定された数だけ敵を円形に配置
        for (int32_t i = 0; i < count; ++i) {
            float angle = (2.0f * 3.14159265f * i) / count;
            Vector3 pos = {
                player_->GetWorldPosition().x + std::cos(angle) * distance,
                0.0f,
                player_->GetWorldPosition().z + std::sin(angle) * distance
            };

            // 敵インスタンス生成と初期化
            auto enemy = std::make_unique<Enemy>();
            enemy->SetHP(hp);
            enemy->SetEXP(exp);
            enemy->Initialize();
            enemy->SetPlayer(player_);
            enemy->SetPosition(pos);
            enemy->SetModelByType(type);
            enemies_.push_back(std::move(enemy));
        }
    }

    file.close();
}

void EnemyManager::Update() {
    // 各敵の更新処理
    for (auto& enemy : enemies_) {
        if (enemy->IsActive()) {
            enemy->Update();
        }
        else if (enemy->GetHP() <= 0 && enemy->JustDied()) {
            // 死亡直後だけ煙パーティクル生成
            const int particleCount = 5;
            for (int i = 0; i < particleCount; ++i) {
                auto p = std::make_unique<DeathParticle>();
                p->Initialize(enemy->GetPosition());
                deathParticles_.push_back(std::move(p));
            }
            enemy->ResetJustDied();
        }
    }

    // パーティクル更新
    for (auto it = deathParticles_.begin(); it != deathParticles_.end();) {
        (*it)->Update();
        if (!(*it)->IsActive()) {
            it = deathParticles_.erase(it);
        }
        else {
            ++it;
        }
    }

    // 敵同士の衝突判定と分離処理
    const float kMinDist = 3.0f;
    const float kPushStrength = 1.0f;

    for (size_t i = 0; i < enemies_.size(); ++i) {
        Enemy* a = enemies_[i].get();
        if (!a->IsActive()) continue;

        for (size_t j = i + 1; j < enemies_.size(); ++j) {
            Enemy* b = enemies_[j].get();
            if (!b->IsActive()) continue;

            Vector3 posA = a->GetPosition();
            Vector3 posB = b->GetPosition();

            float dx = posB.x - posA.x;
            float dz = posB.z - posA.z;
            float distSq = dx * dx + dz * dz;

            if (distSq < kMinDist * kMinDist && distSq > 0.0001f) {
                float dist = std::sqrt(distSq);
                float overlap = kMinDist - dist;

                float nx = dx / dist;
                float nz = dz / dist;

                posA.x -= nx * overlap * kPushStrength;
                posA.z -= nz * overlap * kPushStrength;
                posB.x += nx * overlap * kPushStrength;
                posB.z += nz * overlap * kPushStrength;

                a->SetPosition(posA);
                b->SetPosition(posB);
            }
        }
    }
}

void EnemyManager::Draw(Camera* camera) {
    // アクティブな敵のみ描画
    for (auto& enemy : enemies_) {
        if (enemy->IsActive()) {
            enemy->Draw(camera);
        }
    }

    // 死亡パーティクル描画
    for (auto& p : deathParticles_) {
        p->Draw(camera);
    }
}