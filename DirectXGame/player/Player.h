#pragma once

#include <KamataEngine.h>
#include <memory>

class PlayerManager;

class Player {
public:
    void Initialize();
    void Update();
    void Draw();

    // 基本情報
    KamataEngine::Vector3 GetWorldPosition() const { return worldTransform_.translation_; }
    float GetWorldRotationY() const { return worldTransform_.rotation_.y; }
    KamataEngine::Camera& GetCamera() { return camera_; }

    // 可視制御（外部から点滅制御するためのセッター）
    void SetVisible(bool visible) { visible_ = visible; }

private:
    KamataEngine::Input* input_ = nullptr;
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Camera camera_;
    std::unique_ptr<KamataEngine::Model> playerModel_;

    bool visible_ = true;
    float effectTimer_ = 0.0f;
    static constexpr float kEffectInterval = 0.2f;

    void UpdateMovement();
    void UpdateCamera();
};