#pragma once
#include <unordered_map>
#include <string>
#include <KamataEngine.h>

struct HitBoxData {
    float radius;
    float height;
};

class CollisionManager {
public:
    static CollisionManager* GetInstance();

    void LoadHitBoxCSV(const std::string& filename);
    const HitBoxData& GetHitBoxData(const std::string& name);

    bool CheckCollisionCircle(const KamataEngine::Vector3& posA, float radiusA,
        const KamataEngine::Vector3& posB, float radiusB);

private:
    CollisionManager() = default;
    std::unordered_map<std::string, HitBoxData> hitBoxTable_;
};