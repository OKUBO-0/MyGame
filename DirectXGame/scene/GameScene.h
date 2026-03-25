#pragma once

#include "IScene.h"
#include "GameLevelUpController.h"
#include "GamePauseController.h"
#include "GameStartController.h"
#include "../player/Player.h"
#include "../player/PlayerManager.h"
#include "../enemy/EnemyManager.h"
#include "GridPlane.h"
#include "SkyDome.h"
#include "CurtainTransition.h"
#include "../ui/ExpGauge.h"
#include "../ui/HpGauge.h"
#include "../ui/Timer.h"
#include "../ui/KeyUI.h"
#include <KamataEngine.h>
#include <cstdint>
#include <memory>
#include <list>

class GameScene : public IScene {
public:
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

    bool IsFinished() const override { return finished_; }

private:
    void InitializeAudio();
    void InitializeSceneObjects();
    void InitializeUI();

    bool UpdateCurtainOpening();
    bool UpdateStartWaiting();
    bool UpdatePauseState();
    bool UpdateLevelUpFlow();
    bool UpdateGameTimer();
    bool UpdateDeathFlow();
    bool FinalizeResultTransition();

    void StartResultTransition();
    void UpdateStatusUI();
    void UpdateGameplay();
    void DrawWorld();
    void DrawUI();

    KamataEngine::DirectXCommon* dxCommon_ = nullptr;
    KamataEngine::Input* input_ = nullptr;
    KamataEngine::Audio* audio_ = nullptr;
    KamataEngine::Camera camera_;

    GameStartController startController_;

    std::unique_ptr<Player> player_;
    std::unique_ptr<PlayerManager> playerManager_;
    EnemyManager enemyManager_;

    CurtainTransition curtain_;
    bool curtainCloseStarted_ = false;
    bool curtainOpening_ = true;
    bool finished_ = false;

    GamePauseController pauseController_;

    // Wave 関連は削除済み

    std::unique_ptr<KamataEngine::Sprite> deathOverlay_;
    float deathAlpha_ = 0.0f;
    bool deathFadeInStarted_ = false;
    bool deathFadeInComplete_ = false;
    bool gameStopped_ = false;

    GameLevelUpController levelUpController_;

    std::unique_ptr<KeyUI> keyUI_;

    std::unique_ptr<ExpGauge> expGauge_;
    std::unique_ptr<HpGauge> hpGauge_;

    std::unique_ptr<GridPlane> gridPlane_;
    std::unique_ptr<SkyDome> skyDome_;

    uint32_t pauseSEHandle_ = 0;
    uint32_t startSEHandle_ = 0;
    uint32_t damageSEHandle_ = 0;
    uint32_t deathSEHandle_ = 0;
    uint32_t levelUpSEHandle_ = 0;

    // 追加：制限時間
    float gameTime_ = 0.0f;   // 0秒スタート
    float gameTimeLimit_ = 60.0f; // 制限時間
    std::unique_ptr<Timer> timer_;
};
