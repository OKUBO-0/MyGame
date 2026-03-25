#pragma once

#include <KamataEngine.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class PlayerManager;

struct LevelUpOption {
    std::string name;
    std::function<void(PlayerManager*)> action;
    std::function<uint32_t(PlayerManager*)> getTexture;
    float weight = 1.0f;
};

class GameLevelUpController {
public:
    void Initialize();
    void RegisterDefaultOptions();

    bool TryStart(PlayerManager* playerManager, KamataEngine::Audio* audio, uint32_t levelUpSEHandle);
    bool Update(PlayerManager* playerManager, KamataEngine::Input* input, KamataEngine::Audio* audio,
                uint32_t moveSEHandle, uint32_t decideSEHandle);
    void Draw() const;
    void Reset();

    bool IsActive() const { return active_; }

private:
    int32_t PickWeightedOptionIndex() const;

    std::vector<LevelUpOption> options_;
    std::vector<LevelUpOption> currentChoices_;
    std::unique_ptr<KamataEngine::Sprite> overlaySprite_;
    std::unique_ptr<KamataEngine::Sprite> arrowSprite_;
    std::unique_ptr<KamataEngine::Sprite> choiceSprites_[3];
    bool active_ = false;
    int32_t selection_ = 0;
};
