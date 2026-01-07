#include "NormalBullet.h"
using namespace KamataEngine;

void NormalBullet::Initialize(const Vector3& startPos,
    const Vector3& targetPos,
    int power)
{
    Bullet::Initialize(startPos, power);

    // モデル読み込み
    model_ = std::unique_ptr<Model>(Model::CreateFromOBJ("Bullet"));

    // 方向ベクトル計算
    direction_ = {
        targetPos.x - startPos.x,
        targetPos.y - startPos.y,
        targetPos.z - startPos.z
    };

    float len = std::sqrt(direction_.x * direction_.x +
        direction_.y * direction_.y +
        direction_.z * direction_.z);

    if (len > 0.0001f) {
        direction_.x /= len;
        direction_.y /= len;
        direction_.z /= len;
    }

    traveled_ = 0.0f;
    worldTransform_.UpdateMatrix();
}

bool NormalBullet::CanHitEnemy(void* enemyPtr) {
    auto it = hitCooldowns_.find(enemyPtr);
    if (it == hitCooldowns_.end()) return true;
    return (it->second <= 0.0f);
}

void NormalBullet::RegisterHit(void* enemyPtr) {
    hitCooldowns_[enemyPtr] = kHitInterval;
}

void NormalBullet::Update(const Vector3&)
{
    if (!active_) return;

    // 移動
    worldTransform_.translation_.x += direction_.x * speed_;
    worldTransform_.translation_.y += direction_.y * speed_;
    worldTransform_.translation_.z += direction_.z * speed_;

    traveled_ += speed_;

    // 射程チェック
    if (traveled_ >= range_) {
        active_ = false;
        return;
    }

    // クールタイム更新
    const float kDeltaTime = 0.016f;
    for (auto it = hitCooldowns_.begin(); it != hitCooldowns_.end();) {
        it->second -= kDeltaTime;
        if (it->second <= 0.0f) {
            it = hitCooldowns_.erase(it);
        }
        else {
            ++it;
        }
    }

    worldTransform_.UpdateMatrix();
}

void NormalBullet::Draw(Camera* camera)
{
    Bullet::Draw(camera);
}