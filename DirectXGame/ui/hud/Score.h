#pragma once

#include "../common/UIElement.h"
#include <KamataEngine.h>
#include <cstdint>
#include <memory>
#include <vector>

namespace DirectXGame {

/// <summary>
/// スコアを画面に表示・管理するクラス。
/// 数字スプライトを並べてスコアを描画し、位置やスケールを調整できる。
/// </summary>
class Score : public UIElement {
public:
    /// <summary>初期化処理（数字スプライトの生成やテクスチャの読み込みを行う）</summary>
    void Initialize();

    /// <summary>毎フレーム更新処理（スコア表示に必要な内部状態を更新する）</summary>
    void Update();

    /// <summary>描画処理（現在のスコアを画面に表示する）</summary>
    void Draw() const override;

    /// <summary>表示するスコア値を設定する</summary>
    void SetNumber(int32_t number);

    /// <summary>スコア全体の表示位置を設定する</summary>
    void SetPosition(const KamataEngine::Vector2& pos);

    /// <summary>スコア全体の拡大率を設定する</summary>
    void SetScale(float scale);

private:
    void OnTransformChanged() override;
    void RefreshLayout();
    void UpdateBounds();

public:
    /// <summary>表示桁数を設定する。Initialize 前に呼ぶこと</summary>
    void SetDigitCount(int32_t digitCount);

private:
    int32_t digitCount_ = 5; ///< 最大表示桁数
    std::vector<std::unique_ptr<KamataEngine::Sprite>> sprite_; ///< 各桁ごとのスプライト
    uint32_t textureHandle_ = 0;                              ///< 数字テクスチャのハンドル
    KamataEngine::Vector2 digitSize_ = { 24.0f, 32.0f };        ///< 各数字スプライトのサイズ
    float scale_ = 1.0f;                                      ///< 全体のスケール値
};

} // namespace DirectXGame
