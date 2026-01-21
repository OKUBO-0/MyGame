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

    std::unique_ptr<Score> expUI_; ///< スコア表示UI
    std::unique_ptr<Score> levelUI_;   // ★追加
    std::unique_ptr<Score> killUI_;    // ★追加

    int32_t currentExp_ = 0;       ///< 現在のスコア
    int32_t targetExp_ = 0;      ///< 目標スコア

    int32_t currentLevel_ = 0;     // ★追加
    int32_t targetLevel_ = 0;      // ★追加

    int32_t currentKill_ = 0;      // ★追加
    int32_t targetKill_ = 0;       // ★追加

    uint32_t selectSEHandle_ = 0; ///< 選択SEハンドル
};