#pragma once
#include <KamataEngine.h>
#include <array>
#include <algorithm>
#include <cstdint>
#include <memory>

/// <summary>
/// プレイヤーの経験値を可視化するゲージUIクラス。
/// 現在のEXPと最大EXPを基にゲージを描画し、レベル表示も行う。
/// </summary>
class ExpGauge {
public:
    /// <summary>初期化処理（スプライトやテクスチャの準備を行う）</summary>
    void Initialize();

    /// <summary>毎フレーム更新処理（EXPゲージのアニメーションや表示状態を更新する）</summary>
    void Update();

    /// <summary>描画処理（現在のEXPゲージとレベルを画面に描画する）</summary>
    void Draw();

    /// <summary>プレイヤーのEXPを設定する</summary>
    void SetEXP(int32_t current, int32_t max);

    /// <summary>プレイヤーのレベルを設定する</summary>
    void SetLevel(int32_t level);

    /// <summary>EXPゲージが満タンかどうかを判定する</summary>
    bool IsFilled() const;

private:
    uint32_t dummyTextureHandle_ = 0; ///< ダミーテクスチャハンドル
    uint32_t lvLabelHandle_ = 0;      ///< [LV]ラベル用テクスチャハンドル
    uint32_t lvDigitsHandle_ = 0;     ///< レベル数字用テクスチャハンドル

    std::unique_ptr<KamataEngine::Sprite> yellowFrame_; ///< ゲージ枠スプライト
    std::unique_ptr<KamataEngine::Sprite> blackGauge_;  ///< 背景スプライト
    std::unique_ptr<KamataEngine::Sprite> blueGauge_;   ///< ゲージ本体スプライト

    std::unique_ptr<KamataEngine::Sprite> lvLabel_;     ///< [LV]ラベルスプライト
    static constexpr int32_t kLvDigits = 2;             ///< レベル表示の最大桁数
    std::array<std::unique_ptr<KamataEngine::Sprite>, kLvDigits> sprite_; ///< レベル数字スプライト

    KamataEngine::Vector2 size_ = { 16.0f, 32.0f }; ///< 数字1桁のサイズ

    int32_t displayedExp_ = 0; ///< 表示中のEXP値（アニメーション用）
    int32_t targetExp_ = 0;    ///< 設定されたEXP値
    int32_t maxExp_ = 1;       ///< 最大EXP値
};