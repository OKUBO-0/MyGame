#pragma once

#include "MiniMap.h"
#include <cstdint>
#include <memory>

namespace KamataEngine {
class Audio;
class DirectXCommon;
class Input;
class Sprite;
}

class EnemyManager;
class Player;

class GamePauseController {
public:
    void Initialize();
    bool Update(Player* player, const EnemyManager& enemyManager, KamataEngine::Input* input,
                KamataEngine::Audio* audio, uint32_t toggleSEHandle);
    void Draw(KamataEngine::DirectXCommon* dxCommon) const;
    void Reset();

    bool IsActive() const;
    bool IsGuideActive() const;
    bool ShouldGoResult() const;

private:
    bool active_ = false;
    KamataEngine::Audio* audio_ = nullptr;

    std::unique_ptr<KamataEngine::Sprite> pauseOverlay_;
    std::unique_ptr<KamataEngine::Sprite> guideSprite_;
    std::unique_ptr<KamataEngine::Sprite> cursorSprite_;
    std::unique_ptr<MiniMap> miniMap_;

    uint32_t selectSEHandle_ = 0;
    uint32_t decideSEHandle_ = 0;

    int32_t menuIndex_ = 0;
    bool guideActive_ = false;
    bool goResult_ = false;
};
