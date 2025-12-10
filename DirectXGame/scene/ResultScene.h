#pragma once
#include "IScene.h"
#include "../2d/Fade.h"
#include "../ui/Score.h"
#include <KamataEngine.h>
#include <cstdint>
#include <memory>

/// <summary>
/// リザルトシーンを管理するクラス。
/// スコア表示や演出を行い、ゲーム終了後の画面を制御する。
/// </summary>
class ResultScene : public IScene {
public:
    /// <summary>初期化処理（背景やUI、スコア表示の準備を行う）</summary>
    void Initialize() override;

    /// <summary>毎フレーム更新処理（演出やスコアのアニメーションを更新する）</summary>
    void Update() override;

    /// <summary>描画処理（背景、UI、スコアを画面に描画する）</summary>
    void Draw() override;

    /// <summary>終了処理（リソース解放やシーン終了時の後処理を行う）</summary>
    void Finalize() override;

    /// <summary>シーンが終了状態かどうかを判定する</summary>
    bool IsFinished() const override { return finished_; }

private:
    KamataEngine::DirectXCommon* dxCommon_ = nullptr; ///< DirectX管理（外部から取得）
    KamataEngine::Input* input_ = nullptr;            ///< 入力管理（外部から取得）
    KamataEngine::Audio* audio_ = nullptr;            ///< オーディオ管理（外部から取得）

    std::unique_ptr<KamataEngine::Sprite> backgroundSprite_; ///< 背景スプライト
    std::unique_ptr<KamataEngine::Sprite> resultSprite_;     ///< リザルト文字スプライト
    std::unique_ptr<KamataEngine::Sprite> resultUI_;         ///< リザルトUIスプライト

    Fade fade_;                   ///< フェード演出
    bool fadeOutStarted_ = false; ///< フェードアウト開始フラグ
    bool finished_ = false;       ///< シーン終了フラグ

    std::unique_ptr<Score> scoreUI_; ///< スコア表示UI
    int32_t currentScore_ = 0;       ///< 現在のスコア
    int32_t targetScore_ = 100;      ///< 目標スコア
};