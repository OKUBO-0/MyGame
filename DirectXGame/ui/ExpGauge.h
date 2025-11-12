#pragma once
#include <KamataEngine.h>
#include <array>
#include <algorithm>

class ExpGauge {
public:
    ExpGauge();
    ~ExpGauge();

    void Initialize();
    void Update();
    void Draw();

    void SetEXP(int current, int max); // プレイヤーEXP連動
    void SetLevel(int level);          // レベル表示更新

    // ExpGauge.h
    bool IsFilled() const;

private:
    uint32_t dummyTextureHandle_ = 0;
    uint32_t lvLabelHandle_ = 0;
    uint32_t lvDigitsHandle_ = 0;

    KamataEngine::Sprite* yellowFrame_ = nullptr; // ふち
    KamataEngine::Sprite* blackGauge_ = nullptr;  // 背景
    KamataEngine::Sprite* blueGauge_ = nullptr;   // ゲージ本体

    KamataEngine::Sprite* lvLabel_ = nullptr;     // [LV] ラベル
    static const int lvDigits_ = 2;               // 最大2桁
    std::array<KamataEngine::Sprite*, lvDigits_> sprite_{}; // 数字スプライト

    KamataEngine::Vector2 size_ = { 16.0f, 32.0f }; // 数字1桁のサイズ

    int displayedExp_ = 0;
    int targetExp_ = 0;
    int maxExp_ = 1;
};