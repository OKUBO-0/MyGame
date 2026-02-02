#pragma once

#include "IScene.h"
#include "../player/Player.h"
#include "../enemy/EnemyManager.h"
#include "../3d/GridPlane.h"
#include "../3d/SkyDome.h"
#include "../2d/Fade.h"
#include "../ui/ExpGauge.h"
#include "../ui/HpGauge.h"
#include "../ui/WaveUI.h"
#include <KamataEngine.h>
#include <cstdint>
#include <memory>
#include <list>

/// <summary>
/// ゲームプレイ全体を管理するシーン。
/// プレイヤー、敵、背景、UIなどを統合し、更新・描画・遷移を制御する。
/// </summary>
class GameScene : public IScene {
public:
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
    bool IsFinished() const override { return finished_; }

private:
    KamataEngine::DirectXCommon* dxCommon_ = nullptr; ///< DirectX管理
    KamataEngine::Input* input_ = nullptr;            ///< 入力管理
    KamataEngine::Audio* audio_ = nullptr;            ///< オーディオ管理
    KamataEngine::Camera camera_;                     ///< メインカメラ

    /// <summary>
    /// ゲーム開始演出の状態
    /// Ready: 準備表示 / Go: 開始表示 / Play: プレイ中
    /// </summary>
    enum class StartState { Ready, Go, Play };

    StartState startState_ = StartState::Ready; ///< 開始演出状態
    int32_t startTimer_ = 0;                    ///< 開始演出タイマー

    std::unique_ptr<KamataEngine::Sprite> readyOverlay_; ///< "Ready"表示用スプライト
    std::unique_ptr<KamataEngine::Sprite> goOverlay_;    ///< "Go"表示用スプライト
	std::unique_ptr<KamataEngine::Sprite> guide_; ///< "Guide"表示用スプライト

    std::unique_ptr<Player> player_;           ///< プレイヤー
    EnemyManager enemyManager_;                ///< 敵管理

    Fade fade_;                                ///< フェード演出
    bool fadeOutStarted_ = false;              ///< フェードアウト開始フラグ
    bool finished_ = false;                    ///< シーン終了フラグ

    bool paused_ = false; ///< ポーズ状態
    std::unique_ptr<KamataEngine::Sprite> pauseOverlay_; ///< ポーズ背景
    std::unique_ptr<KamataEngine::Sprite> pauseText_;    ///< ポーズ文字

    int32_t currentWave_ = 1;   ///< 現在のWave番号
    bool waveLoading_ = false;  ///< Wave読み込み中フラグ

    std::unique_ptr<KamataEngine::Sprite> deathOverlay_; ///< 死亡演出スプライト
    float deathAlpha_ = 0.0f;                             ///< 死亡演出アルファ値
    bool deathFadeInStarted_ = false;                     ///< 死亡フェードイン開始フラグ
    bool deathFadeInComplete_ = false;                    ///< 死亡フェードイン完了フラグ
    bool gameStopped_ = false;                            ///< ゲーム停止フラグ

    bool levelUpActive_ = false; ///< レベルアップ演出フラグ
    std::unique_ptr<KamataEngine::Sprite> levelUpOverlay_; ///< レベルアップ演出スプライト

    std::unique_ptr<ExpGauge> expGauge_; ///< 経験値ゲージ
    std::unique_ptr<HpGauge> hpGauge_;   ///< HPゲージ
    bool isGameOver_ = false;            ///< ゲームオーバーフラグ
    std::unique_ptr<WaveUI> waveUI_;     ///< Wave表示UI
    std::unique_ptr<GridPlane> gridPlane_; ///< グリッド背景
    std::unique_ptr<SkyDome> skyDome_;     ///< 天球背景

    std::unique_ptr<KamataEngine::Sprite> arrowSprite_; ///< レベルアップ選択用矢印
    int32_t levelUpSelection_ = 0;                      ///< 現在の選択インデックス
    std::unique_ptr<KamataEngine::Sprite> keyW_;
    std::unique_ptr<KamataEngine::Sprite> keyA_;
    std::unique_ptr<KamataEngine::Sprite> keyS_;
    std::unique_ptr<KamataEngine::Sprite> keyD_;

    std::unique_ptr<KamataEngine::Sprite> enemyIcon_;
    std::unique_ptr<KamataEngine::Sprite> orbIcon_;
    std::unique_ptr<KamataEngine::Sprite> ESC_ui_;

    uint32_t pauseSEHandle_ = 0;
};