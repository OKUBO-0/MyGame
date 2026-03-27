#include "GameLevelUpController.h"
#include "PlayerManager.h"
#include <algorithm>
#include <random>

using namespace KamataEngine;

namespace DirectXGame {

namespace {

std::vector<LevelUpOption> BuildCandidateOptions(const std::vector<LevelUpOption>& baseOptions, PlayerManager* playerManager) {
    std::vector<LevelUpOption> candidates;
    candidates.reserve(baseOptions.size());

    for (const auto& option : baseOptions) {
        LevelUpOption adjusted = option;

        if (option.name == "周囲弾強化" && !playerManager->HasOrbitBullets()) {
            adjusted.weight = 2.2f;
        } else if (option.name == "ドローン" && !playerManager->HasDrone()) {
            adjusted.weight = 1.8f;
        } else if (option.name == "最大HP増加") {
            if (playerManager->GetMaxHP() >= 6) {
                continue;
            }

            adjusted.weight = playerManager->GetMaxHP() <= 4 ? 1.5f : 0.85f;
        } else if (option.name == "移動速度アップ") {
            if (playerManager->GetMoveSpeedLevel() >= 5) {
                continue;
            }

            adjusted.weight = playerManager->GetMoveSpeedLevel() == 0 ? 1.5f : 1.0f;
        } else if (option.name == "HP回復") {
            if (playerManager->GetHP() >= playerManager->GetMaxHP()) {
                continue;
            }

            const float hpRatio =
                static_cast<float>(playerManager->GetHP()) / static_cast<float>((std::max)(1, playerManager->GetMaxHP()));
            adjusted.weight = hpRatio <= 0.34f ? 2.5f : (hpRatio <= 0.67f ? 1.4f : 0.7f);
        }

        candidates.push_back(std::move(adjusted));
    }

    return candidates;
}

} // namespace

void GameLevelUpController::Initialize() {
    uint32_t overlayTexture = TextureManager::Load("ui/game/levelup.png");
    overlaySprite_ = std::unique_ptr<Sprite>(Sprite::Create(overlayTexture, {0, 0}));

    uint32_t arrowTexture = TextureManager::Load("ui/game/arrow.png");
    arrowSprite_ = std::unique_ptr<Sprite>(Sprite::Create(arrowTexture, {0, 0}));
}

void GameLevelUpController::RegisterDefaultOptions() {
    options_.clear();

    options_.push_back({
        "通常弾強化",
        [](PlayerManager* pm) { pm->UpgradeNormalBullets(); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_normal.png"); },
        2.0f
    });

    options_.push_back({
        "周囲弾強化",
        [](PlayerManager* pm) {
            if (!pm->HasOrbitBullets()) {
                pm->AddOrbitBullets();
            } else {
                pm->UpgradeOrbitBullets();
            }
        },
        [](PlayerManager* pm) {
            if (!pm->HasOrbitBullets()) {
                return TextureManager::Load("ui/game/lvup_orbit_add.png");
            }
            return TextureManager::Load("ui/game/lvup_orbit_upgrade.png");
        },
        1.0f
    });

    options_.push_back({
        "ドローン",
        [](PlayerManager* pm) {
            if (!pm->HasDrone()) {
                pm->AddDrone();
            } else {
                pm->UpgradeDrone();
            }
        },
        [](PlayerManager* pm) {
            if (!pm->HasDrone()) {
                return TextureManager::Load("ui/game/lvup_drone_add.png");
            }
            return TextureManager::Load("ui/game/lvup_drone_upgrade.png");
        },
        0.5f
    });

    options_.push_back({
        "攻撃力 +1",
        [](PlayerManager* pm) { pm->UpgradeAttackPower(); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_attack.png"); },
        1.5f
    });

    options_.push_back({
        "移動速度アップ",
        [](PlayerManager* pm) { pm->UpgradeMoveSpeed(); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_speed.png"); },
        1.1f
    });

    options_.push_back({
        "HP回復",
        [](PlayerManager* pm) { pm->RecoverHP(); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_heal.png"); },
        1.0f
    });

    options_.push_back({
        "最大HP増加",
        [](PlayerManager* pm) { pm->IncreaseMaxHP(); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_maxhp.png"); },
        0.8f
    });
}

bool GameLevelUpController::TryStart(PlayerManager* playerManager, Audio* audio, uint32_t levelUpSEHandle) {
    if (!playerManager->IsLevelUpRequested()) {
        return false;
    }

    if (levelUpSEHandle != 0) {
        audio->PlayWave(levelUpSEHandle, false, 1.0f);
    }

    currentChoices_.clear();
    const std::vector<LevelUpOption> candidateOptions = BuildCandidateOptions(options_, playerManager);
    if (candidateOptions.empty()) {
        return false;
    }

    std::vector<int32_t> pickedIndices;
    const int32_t choiceCount = (std::min)(3, static_cast<int32_t>(candidateOptions.size()));
    for (int32_t i = 0; i < choiceCount; ++i) {
        int32_t pickedIndex = 0;
        while (true) {
            pickedIndex = PickWeightedOptionIndex(candidateOptions);
            if (std::find(pickedIndices.begin(), pickedIndices.end(), pickedIndex) == pickedIndices.end()) {
                break;
            }
        }

        pickedIndices.push_back(pickedIndex);
        currentChoices_.push_back(candidateOptions[pickedIndex]);
    }

    for (int32_t i = 0; i < choiceCount; ++i) {
        uint32_t textureHandle = currentChoices_[i].getTexture(playerManager);
        choiceSprites_[i] = std::unique_ptr<Sprite>(
            Sprite::Create(textureHandle, {0.0f, static_cast<float>(i) * 140.0f})
        );
        choiceSprites_[i]->SetSize({1280, 720});
    }
    for (int32_t i = choiceCount; i < 3; ++i) {
        choiceSprites_[i].reset();
    }

    active_ = true;
    selection_ = 0;
    playerManager->ClearLevelUpRequest();
    return true;
}

bool GameLevelUpController::Update(PlayerManager* playerManager, Input* input, Audio* audio,
                                   uint32_t moveSEHandle, uint32_t decideSEHandle) {
    if (!active_) {
        return false;
    }

    int32_t previousSelection = selection_;
    if (input->TriggerKey(DIK_W)) {
        selection_ = std::max<int32_t>(0, selection_ - 1);
    } else if (input->TriggerKey(DIK_S)) {
        selection_ = std::min<int32_t>(2, selection_ + 1);
    }

    if (selection_ != previousSelection && moveSEHandle != 0) {
        audio->PlayWave(moveSEHandle, false, 0.5f);
    }

    arrowSprite_->SetPosition({0.0f, static_cast<float>(selection_) * 140.0f});

    if (input->TriggerKey(DIK_RETURN) || input->TriggerKey(DIK_SPACE)) {
        if (decideSEHandle != 0) {
            audio->PlayWave(decideSEHandle, false, 1.0f);
        }
        currentChoices_[selection_].action(playerManager);
        active_ = false;
    }

    return true;
}

void GameLevelUpController::Draw() const {
    if (!active_) {
        return;
    }

    overlaySprite_->Draw();
    for (const auto& sprite : choiceSprites_) {
        if (sprite) {
            sprite->Draw();
        }
    }
    arrowSprite_->Draw();
}

void GameLevelUpController::Reset() {
    active_ = false;
    selection_ = 0;
    currentChoices_.clear();
    for (auto& sprite : choiceSprites_) {
        sprite.reset();
    }
}

int32_t GameLevelUpController::PickWeightedOptionIndex(const std::vector<LevelUpOption>& candidateOptions) const {
    float totalWeight = 0.0f;
    for (const auto& option : candidateOptions) {
        totalWeight += option.weight;
    }

    static std::mt19937 mt(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, totalWeight);

    float randomValue = dist(mt);
    float accumulatedWeight = 0.0f;
    for (size_t i = 0; i < candidateOptions.size(); ++i) {
        accumulatedWeight += candidateOptions[i].weight;
        if (randomValue <= accumulatedWeight) {
            return static_cast<int32_t>(i);
        }
    }

    return static_cast<int32_t>(candidateOptions.size() - 1);
}

} // namespace DirectXGame
