#pragma once
#include <KamataEngine.h>

struct AABB {
    KamataEngine::Vector3 min; // 最小座標
    KamataEngine::Vector3 max; // 最大座標
};

// AABB同士の衝突判定
inline bool CheckAABBCollision(const AABB& a, const AABB& b) {
    if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
    if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
    if (a.max.z < b.min.z || a.min.z > b.max.z) return false;
    return true;
}