#pragma once

#include "IScene.h"
#include "../player/Player.h"
#include "../player/PlayerManager.h"
#include "../enemy/EnemyManager.h"
#include "../3d/GridPlane.h"
#include "../3d/SkyDome.h"
#include "../2d/CurtainTransition.h"
#include "../ui/ExpGauge.h"
#include "../ui/HpGauge.h"
#include "../ui/Pause.h"
#include <KamataEngine.h>
#include <cstdint>
#include <memory>
#include <list>
#include <functional>
#include <numeric>

struct LevelUpOption {
    std::string name;
    std::function<void(PlayerManager*)> action;
    uint32_t textureHandle;
};

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
    enum class StartState { Wait, Play };
    StartState startState_ = StartState::Wait;
    std::unique_ptr<KamataEngine::Sprite> startOverlay_;

    std::unique_ptr<Player> player_;           ///< プレイヤー
    std::unique_ptr<PlayerManager> playerManager_; ///< プレイヤーマネージャー
    EnemyManager enemyManager_;                ///< 敵管理

    CurtainTransition curtain_;
    bool curtainCloseStarted_ = false;
    bool curtainOpening_ = true;
    bool finished_ = false;

    bool paused_ = false; ///< ポーズ状態
    std::unique_ptr<Pause> pause_;

    int32_t currentWave_ = 1;   ///< 現在のWave番号
    bool waveLoading_ = false;  ///< Wave読み込み中フラグ

    std::unique_ptr<KamataEngine::Sprite> deathOverlay_; ///< 死亡演出スプライト
    float deathAlpha_ = 0.0f;                             ///< 死亡演出アルファ値
    bool deathFadeInStarted_ = false;                     ///< 死亡フェードイン開始フラグ
    bool deathFadeInComplete_ = false;                    ///< 死亡フェードイン完了フラグ
    bool gameStopped_ = false;                            ///< ゲーム停止フラグ

    bool levelUpActive_ = false; ///< レベルアップ演出フラグ
    std::unique_ptr<KamataEngine::Sprite> levelUpOverlay_; ///< レベルアップ演出スプライト
    std::unique_ptr<KamataEngine::Sprite> arrowSprite_; ///< レベルアップ選択用矢印

    std::unique_ptr<KamataEngine::Sprite> keyW_;
    std::unique_ptr<KamataEngine::Sprite> keyA_;
    std::unique_ptr<KamataEngine::Sprite> keyS_;
    std::unique_ptr<KamataEngine::Sprite> keyD_;
    std::unique_ptr<KamataEngine::Sprite> ESC_ui_;

    std::unique_ptr<ExpGauge> expGauge_; ///< 経験値ゲージ
    std::unique_ptr<HpGauge> hpGauge_;   ///< HPゲージ

    std::unique_ptr<GridPlane> gridPlane_; ///< グリッド背景
    std::unique_ptr<SkyDome> skyDome_;     ///< 天球背景

    uint32_t pauseSEHandle_ = 0;

    std::vector<LevelUpOption> levelUpOptions_;   // 全候補
    std::vector<LevelUpOption> currentChoices_;   // 今回の3つ
    std::unique_ptr<KamataEngine::Sprite> choiceSprite_[3];
    int32_t levelUpSelection_ = 0;
};