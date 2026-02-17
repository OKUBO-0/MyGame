#include "Drone.h"
using namespace KamataEngine;

void Drone::Initialize(const Vector3& offset) {
    offset_ = offset;
    worldTransform_.Initialize();
    worldTransform_.scale_ = { 0.5f, 0.5f, 0.5f }; // ★ ドローンを小さくする
    model_ = std::unique_ptr<Model>(Model::CreateFromOBJ("octopus"));
}

void Drone::Update(const Vector3& playerPos,
    const std::vector<std::unique_ptr<Enemy>>& enemies,
    float& fireTimer, float fireInterval, float playerRotationY)
{
    const float dt = 0.016f;

    // ★ プレイヤーの向きに合わせて offset を回転させる（ローカル->ワールド変換）
    // 正しい Y 回転行列を使う:
    // worldRight  = ( cosθ, 0, -sinθ )
    // worldForward = ( sinθ, 0,  cosθ )
    float angle = playerRotationY;

    float cosA = std::cos(angle);
    float sinA = std::sin(angle);

    // ローカルオフセット (offset_.x = 右方向, offset_.z = 前方向) をワールド座標へ変換
    Vector3 rotatedOffset = {
        offset_.x * cosA + offset_.z * sinA,   // x' = cosθ * x + sinθ * z
        offset_.y,
        -offset_.x * sinA + offset_.z * cosA   // z' = -sinθ * x + cosθ * z
    };

    // ★ プレイヤーに追従（向きに合わせた位置）
    worldTransform_.translation_ = {
        playerPos.x + rotatedOffset.x,
        playerPos.y + rotatedOffset.y,
        playerPos.z + rotatedOffset.z
    };

    // --- 以下は今まで通り ---
    fireTimer += dt;

    Enemy* target = nullptr;
    float minDistSq = range_ * range_;

    for (auto& e : enemies) {
        if (!e->IsActive()) continue;

        Vector3 ePos = e->GetPosition();
        float dx = ePos.x - worldTransform_.translation_.x;
        float dz = ePos.z - worldTransform_.translation_.z;
        float distSq = dx * dx + dz * dz;

        if (distSq < minDistSq) {
            minDistSq = distSq;
            target = e.get();
        }
    }

    if (target && fireTimer >= fireInterval) {

        Vector3 dir = {
            target->GetPosition().x - worldTransform_.translation_.x,
            0.0f,
            target->GetPosition().z - worldTransform_.translation_.z
        };

        float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
        if (len > 0.0f) {
            dir.x /= len;
            dir.z /= len;
        }

        auto b = std::make_unique<NormalBullet>();
        b->InitializeForward(worldTransform_.translation_, dir);
        bullets_.push_back(std::move(b));

        fireTimer = 0.0f;
    }

    for (auto& b : bullets_) {
        b->Update(worldTransform_.translation_);
    }

    bullets_.erase(
        std::remove_if(bullets_.begin(), bullets_.end(),
            [](auto& b) { return !b->IsActive(); }),
        bullets_.end()
    );

    worldTransform_.UpdateMatrix();
}

void Drone::Draw(Camera* camera) {
    if (model_) {
        model_->Draw(worldTransform_, *camera);
    }
    for (auto& b : bullets_) {
        b->Draw(camera);
    }
}