#pragma once
#include <KamataEngine.h>
#include "NormalBullet.h"
#include "Enemy.h"
#include <vector>
#include <memory>

class Drone {
public:
    void Initialize(const KamataEngine::Vector3& offset);

    void Update(const KamataEngine::Vector3& playerPos,
        const std::vector<std::unique_ptr<Enemy>>& enemies,
        float& fireTimer, float fireInterval); // ★ 追加

    void Draw(KamataEngine::Camera* camera);

    const std::vector<std::unique_ptr<NormalBullet>>& GetBullets() const { return bullets_; }

private:
    KamataEngine::WorldTransform worldTransform_;
    std::unique_ptr<KamataEngine::Model> model_;

    KamataEngine::Vector3 offset_; // プレイヤーからの相対位置

    float range_ = 25.0f;

    std::vector<std::unique_ptr<NormalBullet>> bullets_;
};
