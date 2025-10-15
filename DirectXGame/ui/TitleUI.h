#pragma once
#include <KamataEngine.h>
class TitleUI {
public:
    TitleUI();
    ~TitleUI();
    void Initialize();
    void Update();
    void Draw();
    void Finalize();

    void SetPositionZ(float z);
    float GetPositionZ() const;

    // --- 回転制御 ---
    void AddRotationY(float angle);

private:
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Camera camera_;
    KamataEngine::Model* playerModel_ = nullptr;
};