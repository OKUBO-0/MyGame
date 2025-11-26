// RippleEffect.h
#pragma once
#include <KamataEngine.h>

class RippleEffect {
public:
    void Initialize(const KamataEngine::Vector3& pos);
    void Update();
    void Draw(KamataEngine::Camera* camera);

    bool IsActive() const { return active_; }

private:
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Model* model_ = nullptr;

    float lifetime_ = 1.0f;   ///< 総寿命（秒）
    float age_ = 0.0f;        ///< 経過時間
    float alpha_ = 1.0f;      ///< 透明度

    float startScale_ = 0.5f; ///< 初期サイズ
    float endScale_ = 1.5f;   ///< 最大サイズ

    bool active_ = true;
};