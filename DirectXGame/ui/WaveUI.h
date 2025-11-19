#pragma once
#include <KamataEngine.h>
#include <array>

class WaveUI {
public:
    WaveUI();
    ~WaveUI();

    void Initialize();
    void SetWave(int wave);   // Wave数を設定
    void Update();
    void Draw();

private:
    static const int kMaxDigits = 2; // 最大2桁（例：10まで）

    KamataEngine::Sprite* waveLabel_ = nullptr; // [WAVE] ラベル
    std::array<KamataEngine::Sprite*, kMaxDigits> digits_{}; // 数字スプライト
    KamataEngine::Vector2 digitSize_ = { 16.0f, 32.0f }; // 数字1桁のサイズ

    uint32_t labelTexHandle_ = 0;
    uint32_t numberTexHandle_ = 0;
};