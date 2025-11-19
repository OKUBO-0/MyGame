#pragma once

#include "IScene.h"
#include "../2d/Fade.h"
#include "../ui/TitleUI.h"
#include <KamataEngine.h>

/// <summary>
/// タイトルシーンを管理するクラス。
/// 背景やUI、演出、BGMを制御し、ゲーム開始前の画面を構成する。
/// </summary>
class TitleScene : public IScene {
public:
    /// <summary>
    /// コンストラクタ
    /// タイトルシーンの初期値を設定する
    /// </summary>
    TitleScene();

    /// <summary>
    /// デストラクタ
    /// 使用したリソースを解放する
    /// </summary>
    ~TitleScene();

    /// <summary>
    /// 初期化処理
    /// 背景スプライトやUI、BGMの準備を行う
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 毎フレーム更新処理
    /// 入力判定や演出更新を行う
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画処理
    /// 背景、UI、演出を画面に描画する
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
    KamataEngine::Sprite* titleSprite_ = nullptr;      ///< タイトル文字スプライト
    KamataEngine::Sprite* titleUISprite_ = nullptr;    ///< タイトルUIスプライト
    TitleUI* titleUI_ = nullptr;                       ///< タイトルUI管理

    Fade fade_;                  ///< フェード演出
    bool fadeOutStarted_ = false; ///< フェードアウト開始フラグ
    bool finished_ = false;       ///< シーン終了フラグ

    uint32_t titleBGMHandle_ = 0; ///< タイトルBGMハンドル
    uint32_t selectSEHandle_ = 0; ///< 選択SEハンドル

    bool modelArrived_ = false; ///< モデル演出到達フラグ
    float modelTargetZ_ = 0.0f; ///< モデルの目標Z座標
    float modelStartZ_ = 30.0f; ///< モデルの開始Z座標
    float modelSpeed_ = -0.2f;  ///< モデルの移動速度

    bool startRotate_ = false;  ///< 回転演出開始フラグ
    float blinkTimer_ = 0.0f;   ///< 点滅演出タイマー
};