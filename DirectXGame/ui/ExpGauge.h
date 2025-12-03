#pragma once
#include <KamataEngine.h>
#include <array>
#include <algorithm>
#include <cstdint> // int32_t

/// <summary>
/// プレイヤーの経験値を可視化するゲージUIクラス。
/// 現在のEXPと最大EXPを基にゲージを描画し、レベル表示も行う。
/// </summary>
class ExpGauge {
public:
    /// <summary>
    /// コンストラクタ
    /// ExpGaugeの初期値を設定する
    /// </summary>
    ExpGauge();

    /// <summary>
    /// デストラクタ
    /// 使用したリソースを解放する
    /// </summary>
    ~ExpGauge();

    /// <summary>
    /// 初期化処理
    /// スプライトやテクスチャの準備を行う
    /// </summary>
    void Initialize();

    /// <summary>
    /// 毎フレーム更新処理
    /// EXPゲージのアニメーションや表示状態を更新する
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// 現在のEXPゲージとレベルを画面に描画する
    /// </summary>
    void Draw();

    /// <summary>
    /// プレイヤーのEXPを設定する
    /// </summary>
    /// <param name="current">現在のEXP値</param>
    /// <param name="max">最大EXP値</param>
    void SetEXP(int32_t current, int32_t max);

    /// <summary>
    /// プレイヤーのレベルを設定する
    /// </summary>
    /// <param name="level">表示するレベル値</param>
    void SetLevel(int32_t level);

    /// <summary>
    /// EXPゲージが満タンかどうかを判定する
    /// </summary>
    /// <returns>true: 満タン / false: 未満</returns>
    bool IsFilled() const;

private:
    uint32_t dummyTextureHandle_ = 0; ///< ダミーテクスチャハンドル
    uint32_t lvLabelHandle_ = 0;      ///< [LV]ラベル用テクスチャハンドル
    uint32_t lvDigitsHandle_ = 0;     ///< レベル数字用テクスチャハンドル

    KamataEngine::Sprite* yellowFrame_ = nullptr; ///< ゲージ枠スプライト
    KamataEngine::Sprite* blackGauge_ = nullptr;  ///< 背景スプライト
    KamataEngine::Sprite* blueGauge_ = nullptr;   ///< ゲージ本体スプライト

    KamataEngine::Sprite* lvLabel_ = nullptr;     ///< [LV]ラベルスプライト
    static constexpr int32_t kLvDigits = 2;       ///< レベル表示の最大桁数
    std::array<KamataEngine::Sprite*, kLvDigits> sprite_{}; ///< レベル数字スプライト

    KamataEngine::Vector2 size_ = { 16.0f, 32.0f }; ///< 数字1桁のサイズ

    int32_t displayedExp_ = 0; ///< 表示中のEXP値（アニメーション用）
    int32_t targetExp_ = 0;    ///< 設定されたEXP値
    int32_t maxExp_ = 1;       ///< 最大EXP値
};