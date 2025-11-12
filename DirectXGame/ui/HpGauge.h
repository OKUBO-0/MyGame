#pragma once
#include <KamataEngine.h>
#include <algorithm>

class HpGauge {
public:
    HpGauge();
    ~HpGauge();

    void Initialize();
    void Update();
    void Draw();

    void SetHP(int current, int max); // プレイヤーHP連動

    // HealthGauge.h
    bool IsDepleted() const;

private:
    uint32_t dummyTextureHandle_ = 0;

    KamataEngine::Sprite* blackGauge_ = nullptr; // 背景
    KamataEngine::Sprite* redGauge_ = nullptr; // HPゲージ

    int displayedHP_ = 0; // 表示中のHP（補間用）
    int targetHP_ = 0;    // 実際のHP
    int maxHP_ = 1;       // 最大HP（0除算防止）
};