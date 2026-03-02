#pragma once
#include <KamataEngine.h>
#include "../ui/MiniMap.h"
#include "../player/Player.h"
#include "../enemy/EnemyManager.h"

class Pause {
public:
    void Initialize();
    void Update(const Player* player, const EnemyManager& enemyManager, KamataEngine::Input* input);
    void Draw();

    void SetActive(bool flag) { active_ = flag; }
    bool IsActive() const { return active_; }

    bool IsGuideActive() const { return guideActive_; }
    bool ShouldGoResult() const { return goResult_; }

    void ResetFlags() { guideActive_ = false; goResult_ = false; }

private:
    bool active_ = false;

    std::unique_ptr<KamataEngine::Sprite> pauseOverlay_;
    std::unique_ptr<KamataEngine::Sprite> guideSprite_;
    std::unique_ptr<KamataEngine::Sprite> cursorSprite_;

    uint32_t pauseTex_ = 0;
    uint32_t guideTex_ = 0;
    uint32_t cursorTex_ = 0;

    std::unique_ptr<MiniMap> miniMap_;

    int32_t menuIndex_ = 0; // 0: Guide, 1: ToResult

    bool guideActive_ = false;
    bool goResult_ = false;
};