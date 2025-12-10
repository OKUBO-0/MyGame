#pragma once
#include <KamataEngine.h>
#include <array>
#include <cstdint>
#include <memory>

/// <summary>
/// Wave数を表示するUIクラス。
/// [WAVE]ラベルと数字スプライトを組み合わせて、現在のWaveを画面に描画する。
/// </summary>
class WaveUI {
public:
    /// <summary>初期化処理（ラベルや数字スプライトの準備を行う）</summary>
    void Initialize();

    /// <summary>Wave数を設定する</summary>
    void SetWave(int32_t wave);

    /// <summary>毎フレーム更新処理（表示状態やアニメーションを更新する）</summary>
    void Update();

    /// <summary>描画処理（Waveラベルと数字を画面に描画する）</summary>
    void Draw();

private:
    static constexpr int32_t kMaxDigits = 2; ///< Wave表示の最大桁数（例：10まで）

    std::unique_ptr<KamataEngine::Sprite> waveLabel_; ///< [WAVE]ラベルスプライト
    std::array<std::unique_ptr<KamataEngine::Sprite>, kMaxDigits> digits_; ///< Wave数字スプライト
    KamataEngine::Vector2 digitSize_ = { 16.0f, 32.0f }; ///< 数字1桁のサイズ

    uint32_t labelTexHandle_ = 0;  ///< ラベル用テクスチャハンドル
    uint32_t numberTexHandle_ = 0; ///< 数字用テクスチャハンドル
};