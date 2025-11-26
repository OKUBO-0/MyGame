#pragma once
#include "IScene.h"
#include "../2d/Fade.h"
#include "../ui/Score.h"
#include <KamataEngine.h>

/// <summary>
/// リザルトシーンを管理するクラス。
/// スコア表示や演出を行い、ゲーム終了後の画面を制御する。
/// </summary>
class ResultScene : public IScene {
public:
    /// <summary>
    /// コンストラクタ
    /// リザルトシーンの初期値を設定する
    /// </summary>
    ResultScene();

    /// <summary>
    /// デストラクタ
    /// 使用したリソースを解放する
    /// </summary>
    ~ResultScene();

    /// <summary>
    /// 初期化処理
    /// 背景やUI、スコア表示の準備を行う
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 毎フレーム更新処理
    /// 演出やスコアのアニメーションを更新する
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画処理
    /// 背景、UI、スコアを画面に描画する
    /// </summary>
    void Draw() override;

    /// <summary>
    /// 終了処理
    /// リソース解放やシーン終了時の後処理を行う
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// シーンが終了状態かどうかを判定する
    /// </summary>
    /// <returns>true: 終了 / false: 継続</returns>
    bool IsFinished() const override { return finished_; }

private:
    KamataEngine::DirectXCommon* dxCommon_ = nullptr; ///< DirectX管理
    KamataEngine::Input* input_ = nullptr;            ///< 入力管理
    KamataEngine::Audio* audio_ = nullptr;            ///< オーディオ管理

    KamataEngine::Sprite* backgroundSprite_ = nullptr; ///< 背景スプライト
    KamataEngine::Sprite* resultSprite_ = nullptr;     ///< リザルト文字スプライト
    KamataEngine::Sprite* resultUI_ = nullptr;         ///< リザルトUIスプライト

    Fade fade_;                  ///< フェード演出
    bool fadeOutStarted_ = false; ///< フェードアウト開始フラグ
    bool finished_ = false;       ///< シーン終了フラグ

    Score* scoreUI_ = nullptr; ///< スコア表示UI
    int currentScore_ = 0;     ///< 現在のスコア
    int targetScore_ = 100;    ///< 目標スコア
};