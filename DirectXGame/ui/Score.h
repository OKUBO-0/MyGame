#pragma once

#include <array>
#include <KamataEngine.h>

/// <summary>
/// スコアを画面に表示・管理するクラス。
/// 数字スプライトを並べてスコアを描画し、位置やスケールを調整できる。
/// </summary>
class Score
{
public:
    /// <summary>
    /// コンストラクタ
    /// スコア表示用の初期値を設定する
    /// </summary>
    Score();

    /// <summary>
    /// デストラクタ
    /// 使用したリソースを解放する
    /// </summary>
    ~Score();

    /// <summary>
    /// 初期化処理
    /// 数字スプライトの生成やテクスチャの読み込みを行う
    /// </summary>
    void Initialize();

    /// <summary>
    /// 毎フレーム更新処理
    /// スコア表示に必要な内部状態を更新する
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// 現在のスコアを画面に表示する
    /// </summary>
    void Draw();

    /// <summary>
    /// 表示するスコア値を設定する
    /// </summary>
    /// <param name="number">表示したいスコア値</param>
    void SetNumber(int number);

    /// <summary>
    /// スコア全体の表示位置を設定する
    /// </summary>
    /// <param name="pos">左端（または基準）位置</param>
    void SetPosition(const KamataEngine::Vector2& pos);

    /// <summary>
    /// スコア全体の拡大率を設定する
    /// </summary>
    /// <param name="scale">拡大率（1.0fで等倍）</param>
    void SetScale(float scale);

private:
    static const int kDigitCount = 5;                         ///< 最大表示桁数
    std::array<KamataEngine::Sprite*, kDigitCount> sprite_{}; ///< 各桁ごとのスプライト
    uint32_t textureHandle_ = 0;                              ///< 数字テクスチャのハンドル
    KamataEngine::Vector2 size_ = { 16.0f, 32.0f };           ///< 各数字スプライトのサイズ
    KamataEngine::Vector2 basePosition_ = { 800.0f, 10.0f };  ///< スコア全体の基準位置
    float scale_ = 1.0f;                                      ///< 全体のスケール値
};