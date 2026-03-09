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
#include "../ui/Timer.h"
#include "../ui/KeyUI.h"
#include <KamataEngine.h>
#include <cstdint>
#include <memory>
#include <list>
#include <functional>
#include <numeric>

struct LevelUpOption {
    std::string name;
    std::function<void(PlayerManager*)> action;
    std::function<uint32_t(PlayerManager*)> getTexture;
    float weight = 1.0f;
};

class GameScene : public IScene {
public:
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

    bool IsFinished() const override { return finished_; }

private:
    KamataEngine::DirectXCommon* dxCommon_ = nullptr;
    KamataEngine::Input* input_ = nullptr;
    KamataEngine::Audio* audio_ = nullptr;
    KamataEngine::Camera camera_;

    enum class StartState { Wait, Play };
    StartState startState_ = StartState::Wait;
    std::unique_ptr<KamataEngine::Sprite> startOverlay_;

    std::unique_ptr<Player> player_;
    std::unique_ptr<PlayerManager> playerManager_;
    EnemyManager enemyManager_;

    CurtainTransition curtain_;
    bool curtainCloseStarted_ = false;
    bool curtainOpening_ = true;
    bool finished_ = false;

    bool paused_ = false;
    std::unique_ptr<Pause> pause_;

    // Wave 関連は削除済み

    std::unique_ptr<KamataEngine::Sprite> deathOverlay_;
    float deathAlpha_ = 0.0f;
    bool deathFadeInStarted_ = false;
    bool deathFadeInComplete_ = false;
    bool gameStopped_ = false;

    bool levelUpActive_ = false;
    std::unique_ptr<KamataEngine::Sprite> levelUpOverlay_;
    std::unique_ptr<KamataEngine::Sprite> arrowSprite_;

    std::unique_ptr<KeyUI> keyUI_;

    std::unique_ptr<ExpGauge> expGauge_;
    std::unique_ptr<HpGauge> hpGauge_;

    std::unique_ptr<GridPlane> gridPlane_;
    std::unique_ptr<SkyDome> skyDome_;

    uint32_t pauseSEHandle_ = 0;

    std::vector<LevelUpOption> levelUpOptions_;
    std::vector<LevelUpOption> currentChoices_;
    std::unique_ptr<KamataEngine::Sprite> choiceSprite_[3];
    int32_t levelUpSelection_ = 0;

    // 追加：制限時間
    float gameTime_ = 0.0f;   // 0秒スタート
    float gameTimeLimit_ = 60.0f; // 制限時間
    std::unique_ptr<Timer> timer_;
};