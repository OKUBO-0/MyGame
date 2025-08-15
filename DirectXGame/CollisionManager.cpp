#include "CollisionManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

CollisionManager* CollisionManager::GetInstance() {
    static CollisionManager instance;
    return &instance;
}

void CollisionManager::LoadHitBoxCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "CSVファイルを開けませんでした: " << filename << "\n";
        return;
    }

    std::string line;
    bool firstLine = true;
    while (std::getline(file, line)) {
        if (firstLine) { // ヘッダー行スキップ
            firstLine = false;
            continue;
        }

        std::stringstream ss(line);
        std::string name, radiusStr, heightStr;

        std::getline(ss, name, ',');
        std::getline(ss, radiusStr, ',');
        std::getline(ss, heightStr, ',');

        HitBoxData data;
        data.radius = std::stof(radiusStr);
        data.height = std::stof(heightStr);

        hitBoxTable_[name] = data;
    }
}

const HitBoxData& CollisionManager::GetHitBoxData(const std::string& name) {
    return hitBoxTable_[name];
}

bool CollisionManager::CheckCollisionCircle(const KamataEngine::Vector3& posA, float radiusA,
    const KamataEngine::Vector3& posB, float radiusB) {
    float dx = posA.x - posB.x;
    float dz = posA.z - posB.z; // 高さは無視
    float distSq = dx * dx + dz * dz;
    float sumRadius = radiusA + radiusB;
    return distSq <= (sumRadius * sumRadius);
}