#pragma once
#include <KamataEngine.h>
#include <algorithm>

/// <summary>
/// プレイヤーのHPを可視化するゲージUIクラス。
/// 現在のHPと最大HPを基にゲージを描画し、減少時には補間表示を行う。
/// </summary>
class HpGauge {
public:
    /// <summary>
    /// コンストラクタ
    /// HpGaugeの初期値を設定する
    /// </summary>
    HpGauge();

    /// <summary>
    /// デストラクタ
    /// 使用したリソースを解放する
    /// </summary>
    ~HpGauge();

    /// <summary>
    /// 初期化処理
    /// スプライトやテクスチャの準備を行う
    /// </summary>
    void Initialize();

    /// <summary>
    /// 毎フレーム更新処理
    /// HPゲージの補間や表示状態を更新する
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// 現在のHPゲージを画面に描画する
    /// </summary>
    void Draw();

    /// <summary>
    /// プレイヤーのHPを設定する
    /// </summary>
    /// <param name="current">現在のHP値</param>
    /// <param name="max">最大HP値</param>
    void SetHP(int current, int max);

    /// <summary>
    /// HPがゼロになっているかを判定する
    /// </summary>
    /// <returns>true: HPゼロ / false: HPあり</returns>
    bool IsDepleted() const;

private:
    uint32_t dummyTextureHandle_ = 0; ///< ダミーテクスチャハンドル

    KamataEngine::Sprite* blackGauge_ = nullptr; ///< 背景スプライト
    KamataEngine::Sprite* redGauge_ = nullptr;   ///< HPゲージスプライト

    int displayedHP_ = 0; ///< 表示中のHP（補間用）
    int targetHP_ = 0;    ///< 実際のHP
    int maxHP_ = 1;       ///< 最大HP（0除算防止）
};