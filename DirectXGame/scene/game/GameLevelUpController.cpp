#include "GameLevelUpController.h"
#include "../../core/InputBindings.h"
#include "../../player/core/PlayerManager.h"
#include "../../ui/common/UILayoutIO.h"
#include <algorithm>
#include <random>

using namespace KamataEngine;

namespace DirectXGame {

namespace {

const char* kLevelUpLayoutPath = "Resources/data/ui_layout_levelup.csv";

std::vector<LevelUpOption> BuildCandidateOptions(const std::vector<LevelUpOption>& baseOptions, PlayerManager* playerManager) {
    std::vector<LevelUpOption> candidates;
    candidates.reserve(baseOptions.size());

    for (const auto& option : baseOptions) {
        LevelUpOption adjusted = option;

        if (option.name == "周囲弾強化" && !playerManager->HasOrbitBullets()) {
            adjusted.weight = 2.2f;
        } else if (option.name == "通常弾強化") {
            const float interval = playerManager->GetNormalBulletInterval();
            if (interval <= 0.24f) {
                adjusted.weight = 0.35f;
            } else if (interval <= 0.32f) {
                adjusted.weight = 0.7f;
            } else if (interval <= 0.45f) {
                adjusted.weight = 1.1f;
            }
        } else if (option.name == "ドローン" && !playerManager->HasDrone()) {
            adjusted.weight = 1.8f;
        } else if (option.name == "攻撃力 +1") {
            const int attackPower = playerManager->GetAttackPower();
            adjusted.weight = attackPower >= 6 ? 0.75f : (attackPower >= 4 ? 1.1f : 1.5f);
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
    overlaySprite_->SetSize({1280.0f, 720.0f});

    uint32_t arrowTexture = TextureManager::Load("ui/game/arrow.png");
    arrowSprite_ = std::unique_ptr<Sprite>(Sprite::Create(arrowTexture, {0, 0}));
    {
        const auto layout = UILayoutIO::Load(kLevelUpLayoutPath);
        for (int i = 0; i < 3; ++i) {
            const std::string key = "choicePosition" + std::to_string(i);
            if (const auto it = layout.find(key); it != layout.end() && it->second.size() >= 2) {
                layoutSettings_.choicePositions[i] = { it->second[0], it->second[1] };
            }
        }
        if (const auto it = layout.find("choiceSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.choiceSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("arrowBasePosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.arrowBasePosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("choiceSpacingY"); it != layout.end() && !it->second.empty()) {
            layoutSettings_.choiceSpacingY = it->second[0];
        }
    }
    ApplyLayout();
}

void GameLevelUpController::RegisterDefaultOptions() {
    options_.clear();

    options_.push_back({
        "通常弾強化",
        [](PlayerManager* pm) { pm->UpgradeNormalBullets(); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_normal.png"); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_normal_icon.png"); },
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
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_orbit_icon.png"); },
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
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_drone_icon.png"); },
        0.5f
    });

    options_.push_back({
        "攻撃力 +1",
        [](PlayerManager* pm) { pm->UpgradeAttackPower(); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_attack.png"); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_attack_icon.png"); },
        1.5f
    });

    options_.push_back({
        "移動速度アップ",
        [](PlayerManager* pm) { pm->UpgradeMoveSpeed(); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_speed.png"); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_speed_icon.png"); },
        1.1f
    });

    options_.push_back({
        "HP回復",
        [](PlayerManager* pm) { pm->RecoverHP(); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_heal.png"); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_heal_icon.png"); },
        1.0f
    });

    options_.push_back({
        "最大HP増加",
        [](PlayerManager* pm) { pm->IncreaseMaxHP(); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_maxhp.png"); },
        [](PlayerManager*) { return TextureManager::Load("ui/game/lvup_maxhp_icon.png"); },
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
            Sprite::Create(textureHandle, layoutSettings_.choicePositions[i])
        );
        choiceSprites_[i]->SetSize(layoutSettings_.choiceSize);

        uint32_t iconTextureHandle = currentChoices_[i].getIconTexture(playerManager);
        iconSprites_[i] = std::unique_ptr<Sprite>(
            Sprite::Create(iconTextureHandle, layoutSettings_.choicePositions[i])
        );
        iconSprites_[i]->SetSize(layoutSettings_.choiceSize);
    }
    for (int32_t i = choiceCount; i < 3; ++i) {
        choiceSprites_[i].reset();
        iconSprites_[i].reset();
    }

    active_ = true;
    selection_ = 0;
    ApplyLayout();
    playerManager->ClearLevelUpRequest();
    return true;
}

bool GameLevelUpController::Update(PlayerManager* playerManager, Input* input, Audio* audio,
                                   uint32_t moveSEHandle, uint32_t decideSEHandle) {
    if (!active_) {
        return false;
    }

    int32_t previousSelection = selection_;
    if (InputBindings::IsMenuUpTriggered(input)) {
        selection_ = std::max<int32_t>(0, selection_ - 1);
    } else if (InputBindings::IsMenuDownTriggered(input)) {
        selection_ = std::min<int32_t>(2, selection_ + 1);
    }

    if (selection_ != previousSelection && moveSEHandle != 0) {
        audio->PlayWave(moveSEHandle, false, 0.5f);
    }

    arrowSprite_->SetPosition({layoutSettings_.arrowBasePosition.x,
                               layoutSettings_.arrowBasePosition.y + static_cast<float>(selection_) * layoutSettings_.choiceSpacingY});

    if (InputBindings::IsConfirmTriggered(input)) {
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
    for (const auto& sprite : iconSprites_) {
        if (sprite) {
            sprite->Draw();
        }
    }
    arrowSprite_->Draw();
}

void GameLevelUpController::DebugDrawImGui() {
#ifdef _DEBUG
    if (!ImGui::Begin("UI Debug")) {
        ImGui::End();
        return;
    }

    if (ImGui::CollapsingHeader("Level Up", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Enable LevelUp Debug", &layoutSettings_.debugEnabled);
        if (layoutSettings_.debugEnabled) {
            float choiceSize[2]{ layoutSettings_.choiceSize.x, layoutSettings_.choiceSize.y };
            if (ImGui::DragFloat2("Choice Size", choiceSize, 1.0f, 64.0f, 1280.0f)) {
                layoutSettings_.choiceSize = { choiceSize[0], choiceSize[1] };
                ApplyLayout();
            }

            for (int i = 0; i < 3; ++i) {
                float choicePosition[2]{ layoutSettings_.choicePositions[i].x, layoutSettings_.choicePositions[i].y };
                const std::string label = "Choice " + std::to_string(i + 1);
                if (ImGui::DragFloat2(label.c_str(), choicePosition, 1.0f, -400.0f, 1280.0f)) {
                    layoutSettings_.choicePositions[i] = { choicePosition[0], choicePosition[1] };
                    ApplyLayout();
                }
            }

            float arrowBase[2]{ layoutSettings_.arrowBasePosition.x, layoutSettings_.arrowBasePosition.y };
            if (ImGui::DragFloat2("Arrow Base", arrowBase, 1.0f, -400.0f, 1280.0f)) {
                layoutSettings_.arrowBasePosition = { arrowBase[0], arrowBase[1] };
                ApplyLayout();
            }

            if (ImGui::DragFloat("Choice Spacing", &layoutSettings_.choiceSpacingY, 1.0f, 16.0f, 320.0f)) {
                ApplyLayout();
            }

            if (ImGui::Button("Save LevelUp Layout")) {
                UILayoutIO::Save(kLevelUpLayoutPath, {
                    { "choicePosition0", { layoutSettings_.choicePositions[0].x, layoutSettings_.choicePositions[0].y } },
                    { "choicePosition1", { layoutSettings_.choicePositions[1].x, layoutSettings_.choicePositions[1].y } },
                    { "choicePosition2", { layoutSettings_.choicePositions[2].x, layoutSettings_.choicePositions[2].y } },
                    { "choiceSize", { layoutSettings_.choiceSize.x, layoutSettings_.choiceSize.y } },
                    { "arrowBasePosition", { layoutSettings_.arrowBasePosition.x, layoutSettings_.arrowBasePosition.y } },
                    { "choiceSpacingY", { layoutSettings_.choiceSpacingY } },
                });
            }
        }
    }

    ImGui::End();
#endif
}

void GameLevelUpController::Reset() {
    active_ = false;
    selection_ = 0;
    currentChoices_.clear();
    for (auto& sprite : choiceSprites_) {
        sprite.reset();
    }
    for (auto& sprite : iconSprites_) {
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

void GameLevelUpController::ApplyLayout() {
    if (arrowSprite_) {
        arrowSprite_->SetPosition({layoutSettings_.arrowBasePosition.x,
                                   layoutSettings_.arrowBasePosition.y + static_cast<float>(selection_) * layoutSettings_.choiceSpacingY});
    }

    for (int i = 0; i < 3; ++i) {
        if (!choiceSprites_[i]) {
        } else {
            choiceSprites_[i]->SetPosition(layoutSettings_.choicePositions[i]);
            choiceSprites_[i]->SetSize(layoutSettings_.choiceSize);
        }

        if (!iconSprites_[i]) {
            continue;
        }

        iconSprites_[i]->SetPosition(layoutSettings_.choicePositions[i]);
        iconSprites_[i]->SetSize(layoutSettings_.choiceSize);
    }
}

} // namespace DirectXGame
