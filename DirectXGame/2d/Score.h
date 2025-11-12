#pragma once

#include <array>
#include <KamataEngine.h>

/// スコアを画面に表示するクラス
class Score
{
public:
    /// コンストラクタ
    Score();

    /// デストラクタ
    ~Score();

    /// 初期化処理
    void Initialize();

    /// 毎フレーム更新処理
    void Update();

    /// 描画処理
    void Draw();

    /// 表示するスコア値を設定
    /// 表示したい数値
    void SetNumber(int number);

    /// スコア全体の表示位置を設定
    /// 左端（または基準）位置
    void SetPosition(const KamataEngine::Vector2& pos);

    /// スコア全体の拡大率を設定
    /// スケール値
    void SetScale(float scale);

private:
    /// 最大表示桁数
    static const int kDigitCount = 5;

    /// 各桁ごとのスプライト
    std::array<KamataEngine::Sprite*, kDigitCount> sprite_{};

    /// 数字テクスチャのハンドル
    uint32_t textureHandle_ = 0;

    /// 各数字スプライトのサイズ
    KamataEngine::Vector2 size_ = { 32.0f, 64.0f };

    /// スコア全体の基準位置
    KamataEngine::Vector2 basePosition_ = { 800.0f, 10.0f };

    /// 全体のスケール値
    float scale_ = 1.0f;
};