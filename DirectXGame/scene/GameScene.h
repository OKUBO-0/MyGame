#pragma once

#include "IScene.h"
#include "../player/Player.h"
#include "../enemy/EnemyManager.h"
#include "../3d/SkyDome.h"
#include "../2d/Fade.h"
#include "../ui/ExpGauge.h"
#include "../ui/HpGauge.h"
#include "../ui/WaveUI.h"
#include <KamataEngine.h>

/// <summary>
/// ゲームプレイ全体を管理するシーン。
/// プレイヤー、敵、背景、UIなどを統合し、更新・描画・遷移を制御する。
/// </summary>
class GameScene : public IScene {
public:
    /// <summary>
    /// コンストラクタ
    /// シーンの初期値を設定する
    /// </summary>
    GameScene();

    /// <summary>
    /// デストラクタ
    /// 使用したリソースを解放する
    /// </summary>
    ~GameScene();

    /// <summary>
    /// 初期化処理
    /// ゲームオブジェクトやUIの生成・設定を行う
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 毎フレーム更新処理
    /// プレイヤー、敵、UI、演出などを更新する
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画処理
    /// 背景、プレイヤー、敵、UIを画面に描画する
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
    KamataEngine::Camera camera_;                     ///< メインカメラ

    /// <summary>
    /// ゲーム開始演出の状態
    /// Ready: 準備表示
    /// Go: 開始表示
    /// Play: プレイ中
    /// </summary>
    enum class StartState {
        Ready,
        Go,
        Play
    };

    StartState startState_ = StartState::Ready; ///< 開始演出状態
    int startTimer_ = 0;                        ///< 開始演出タイマー

    KamataEngine::Sprite* readyOverlay_ = nullptr; ///< "Ready"表示用スプライト
    KamataEngine::Sprite* goOverlay_ = nullptr;    ///< "Go"表示用スプライト

    SkyDome* skyDome_ = nullptr; ///< 背景の天球
    Player* player_ = nullptr;   ///< プレイヤー
    EnemyManager enemyManager_;  ///< 敵管理

    Fade fade_;                  ///< フェード演出
    bool fadeOutStarted_ = false; ///< フェードアウト開始フラグ
    bool finished_ = false;       ///< シーン終了フラグ

    bool paused_ = false; ///< ポーズ状態
    KamataEngine::Sprite* pauseOverlay_ = nullptr; ///< ポーズ背景
    KamataEngine::Sprite* pauseText_ = nullptr;    ///< ポーズ文字

    int currentWave_ = 1;   ///< 現在のWave番号
    bool waveLoading_ = false; ///< Wave読み込み中フラグ

    KamataEngine::Sprite* deathOverlay_ = nullptr; ///< 死亡演出スプライト
    float deathAlpha_ = 0.0f;                      ///< 死亡演出アルファ値
    bool deathFadeInStarted_ = false;              ///< 死亡フェードイン開始フラグ
    bool deathFadeInComplete_ = false;             ///< 死亡フェードイン完了フラグ
    bool gameStopped_ = false;                     ///< ゲーム停止フラグ

    bool levelUpActive_ = false; ///< レベルアップ演出フラグ
    KamataEngine::Sprite* levelUpOverlay_ = nullptr; ///< レベルアップ演出スプライト

    ExpGauge* expGauge_ = nullptr; ///< 経験値ゲージ
    HpGauge* hpGauge_ = nullptr;   ///< HPゲージ
    bool isGameOver_ = false;      ///< ゲームオーバーフラグ
    WaveUI* waveUI_ = nullptr;     ///< Wave表示UI
};