#include "GamePauseController.h"
#include "EnemyManager.h"
#include "../../player/core/PlayerManager.h"
#include "../../ui/common/UILayoutIO.h"
#include "Player.h"
#include <algorithm>
#include <KamataEngine.h>

using namespace KamataEngine;

namespace DirectXGame {

namespace {

const char* kPauseLayoutPath = "Resources/data/ui_layout_pause.csv";

}

void GamePauseController::Initialize() {
    audio_ = Audio::GetInstance();

    uint32_t pauseTexture = TextureManager::Load("ui/game/pause.png");
    pauseOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(pauseTexture, {0, 0}));
    pauseOverlay_->SetSize({1280, 720});

    uint32_t guideTexture = TextureManager::Load("ui/title/guideUI.png");
    guideSprite_ = std::unique_ptr<Sprite>(Sprite::Create(guideTexture, {0, 0}));
    guideSprite_->SetSize({1280, 720});
    guideSprite_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});

    uint32_t cursorTexture = TextureManager::Load("ui/game/pause_arrow.png");
    cursorSprite_ = std::unique_ptr<Sprite>(Sprite::Create(cursorTexture, {0, 0}));
    cursorSprite_->SetSize({1280, 720});

    miniMap_ = std::make_unique<MiniMap>();
    miniMap_->Initialize();

    statsPanel_ = std::make_unique<UIPanel>();
    statsPanel_->Initialize();
    statsPanel_->SetColor({0.0f, 0.0f, 0.0f, 0.72f});

    killIconLabel_ = std::make_unique<UILabel>();
    killIconLabel_->Initialize(TextureManager::Load("ui/game/minimap_enemy.png"), layoutSettings_.killIconLocalPosition);
    killIconLabel_->SetParent(statsPanel_.get());
    killIconLabel_->SetSize({28, 28});

    buildNormalLabel_ = std::make_unique<UILabel>();
    buildNormalLabel_->Initialize(TextureManager::Load("ui/game/lvup_normal.png"), layoutSettings_.buildRowLocalPosition);
    buildNormalLabel_->SetParent(statsPanel_.get());
    buildNormalLabel_->SetSize(layoutSettings_.buildIconSize);

    buildOrbitLabel_ = std::make_unique<UILabel>();
    buildOrbitLabel_->Initialize(TextureManager::Load("ui/game/lvup_orbit_add.png"),
                                 {layoutSettings_.buildRowLocalPosition.x + layoutSettings_.buildStepX, layoutSettings_.buildRowLocalPosition.y});
    buildOrbitLabel_->SetParent(statsPanel_.get());
    buildOrbitLabel_->SetSize(layoutSettings_.buildIconSize);

    buildDroneLabel_ = std::make_unique<UILabel>();
    buildDroneLabel_->Initialize(TextureManager::Load("ui/game/lvup_drone_add.png"),
                                 {layoutSettings_.buildRowLocalPosition.x + layoutSettings_.buildStepX * 2.0f, layoutSettings_.buildRowLocalPosition.y});
    buildDroneLabel_->SetParent(statsPanel_.get());
    buildDroneLabel_->SetSize(layoutSettings_.buildIconSize);

    buildAttackLabel_ = std::make_unique<UILabel>();
    buildAttackLabel_->Initialize(TextureManager::Load("ui/game/lvup_attack.png"),
                                  {layoutSettings_.buildRowLocalPosition.x + layoutSettings_.buildStepX * 3.0f, layoutSettings_.buildRowLocalPosition.y});
    buildAttackLabel_->SetParent(statsPanel_.get());
    buildAttackLabel_->SetSize(layoutSettings_.buildIconSize);

    killScore_ = std::make_unique<Score>();
    killScore_->Initialize();
    killScore_->SetScale(1.0f);

    selectSEHandle_ = audio_->LoadWave("audio/se/se_pause.wav");
    decideSEHandle_ = audio_->LoadWave("audio/se/se_exp.wav");

    active_ = false;
    guideActive_ = false;
    guideTransitionState_ = GuideTransitionState::None;
    guideAlpha_ = 0.0f;
    goResult_ = false;
    menuIndex_ = 0;
    {
        const auto layout = UILayoutIO::Load(kPauseLayoutPath);
        if (const auto it = layout.find("statsPanelPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.statsPanelPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("statsPanelSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.statsPanelSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("statsPanelAnchor"); it != layout.end() && !it->second.empty()) {
            layoutSettings_.statsPanelAnchor = static_cast<UIElement::Anchor>(static_cast<int>(it->second[0]));
        }
        if (const auto it = layout.find("killIconLocalPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.killIconLocalPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("buildRowLocalPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.buildRowLocalPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("buildStepX"); it != layout.end() && !it->second.empty()) {
            layoutSettings_.buildStepX = it->second[0];
        }
        if (const auto it = layout.find("buildIconSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.buildIconSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("killScoreLocalPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.killScoreLocalPosition = { it->second[0], it->second[1] };
        }
    }
    ApplyLayout();
    UpdateVisibility();
}

bool GamePauseController::Update(Player* player, const EnemyManager& enemyManager, const PlayerManager& playerManager,
                                 Input* input, Audio* audio, uint32_t toggleSEHandle) {
    if (!guideActive_ && input->TriggerKey(DIK_ESCAPE)) {
        active_ = !active_;
        guideActive_ = false;
        goResult_ = false;
        if (toggleSEHandle != 0) {
            audio->PlayWave(toggleSEHandle, false, 0.5f);
        }
    }

    if (!active_) {
        UpdateVisibility();
        return false;
    }

    if (guideTransitionState_ != GuideTransitionState::None) {
        constexpr float kGuideFadeStep = 0.12f;
        if (guideTransitionState_ == GuideTransitionState::FadeIn) {
            guideAlpha_ += kGuideFadeStep;
            if (guideAlpha_ >= 1.0f) {
                guideAlpha_ = 1.0f;
                guideTransitionState_ = GuideTransitionState::None;
                guideActive_ = true;
            }
        } else {
            guideAlpha_ -= kGuideFadeStep;
            if (guideAlpha_ <= 0.0f) {
                guideAlpha_ = 0.0f;
                guideTransitionState_ = GuideTransitionState::None;
                guideActive_ = false;
            }
        }
        guideSprite_->SetColor({1.0f, 1.0f, 1.0f, guideAlpha_});
        UpdateVisibility();
        return true;
    }

    if (guideActive_) {
        if (input->TriggerKey(DIK_ESCAPE)) {
            if (decideSEHandle_ != 0) {
                audio_->PlayWave(decideSEHandle_, false, 1.0f);
            }
            guideTransitionState_ = GuideTransitionState::FadeOut;
        }
        UpdateVisibility();
        return true;
    }

    miniMap_->Update(player, enemyManager);
    UpdateStats(enemyManager, playerManager);
    UpdateVisibility();

    int32_t previousMenuIndex = menuIndex_;
    if (input->TriggerKey(DIK_W)) {
        menuIndex_ = std::max<int32_t>(0, menuIndex_ - 1);
    }
    if (input->TriggerKey(DIK_S)) {
        menuIndex_ = std::min<int32_t>(1, menuIndex_ + 1);
    }
    if (menuIndex_ != previousMenuIndex && selectSEHandle_ != 0) {
        audio_->PlayWave(selectSEHandle_, false, 1.0f);
    }

    switch (menuIndex_) {
    case 0:
        cursorSprite_->SetPosition({0, 0});
        break;
    case 1:
        cursorSprite_->SetPosition({0, 170});
        break;
    }

    if (input->TriggerKey(DIK_SPACE) || input->TriggerKey(DIK_RETURN)) {
        if (decideSEHandle_ != 0) {
            audio_->PlayWave(decideSEHandle_, false, 1.0f);
        }
        if (menuIndex_ == 0) {
            guideAlpha_ = 0.0f;
            guideSprite_->SetColor({1.0f, 1.0f, 1.0f, guideAlpha_});
            guideTransitionState_ = GuideTransitionState::FadeIn;
        } else if (menuIndex_ == 1) {
            goResult_ = true;
        }
    }

    return true;
}

void GamePauseController::Draw() const {
    if (!active_) {
        return;
    }

    pauseOverlay_->Draw();
    miniMap_->Draw();
    if (guideActive_ || guideTransitionState_ != GuideTransitionState::None) {
        guideSprite_->Draw();
    } else {
        statsPanel_->Draw();
        killIconLabel_->Draw();
        killScore_->Draw();
        buildNormalLabel_->Draw();
        buildOrbitLabel_->Draw();
        buildDroneLabel_->Draw();
        buildAttackLabel_->Draw();
        cursorSprite_->Draw();
    }
}

void GamePauseController::Reset() {
    active_ = false;
    guideActive_ = false;
    guideTransitionState_ = GuideTransitionState::None;
    guideAlpha_ = 0.0f;
    goResult_ = false;
    menuIndex_ = 0;
    guideSprite_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});
    UpdateVisibility();
}

bool GamePauseController::IsActive() const {
    return active_;
}

bool GamePauseController::IsGuideActive() const {
    return guideActive_;
}

bool GamePauseController::ShouldGoResult() const {
    return goResult_;
}

void GamePauseController::DebugDrawImGui() {
#ifdef _DEBUG
    if (!ImGui::Begin("UI Debug")) {
        ImGui::End();
        return;
    }

    ImGui::Checkbox("Pause Layout", &debugLayoutEnabled_);
    if (debugLayoutEnabled_ && ImGui::CollapsingHeader("Pause Menu", ImGuiTreeNodeFlags_DefaultOpen)) {
        float panelPosition[2]{ layoutSettings_.statsPanelPosition.x, layoutSettings_.statsPanelPosition.y };
        if (ImGui::DragFloat2("Panel Position", panelPosition, 1.0f, 0.0f, 1280.0f)) {
            layoutSettings_.statsPanelPosition = { panelPosition[0], panelPosition[1] };
            ApplyLayout();
        }

        float panelSize[2]{ layoutSettings_.statsPanelSize.x, layoutSettings_.statsPanelSize.y };
        if (ImGui::DragFloat2("Panel Size", panelSize, 1.0f, 80.0f, 1280.0f)) {
            layoutSettings_.statsPanelSize = { panelSize[0], panelSize[1] };
            ApplyLayout();
        }

        static const char* kAnchorLabels[] = {
            "TopLeft", "TopCenter", "TopRight",
            "MiddleLeft", "Center", "MiddleRight",
            "BottomLeft", "BottomCenter", "BottomRight"
        };
        int anchorIndex = static_cast<int>(layoutSettings_.statsPanelAnchor);
        if (ImGui::Combo("Panel Anchor", &anchorIndex, kAnchorLabels, IM_ARRAYSIZE(kAnchorLabels))) {
            layoutSettings_.statsPanelAnchor = static_cast<UIElement::Anchor>(anchorIndex);
            ApplyLayout();
        }

        float killIconPosition[2]{ layoutSettings_.killIconLocalPosition.x, layoutSettings_.killIconLocalPosition.y };
        if (ImGui::DragFloat2("Kill Icon", killIconPosition, 1.0f, -400.0f, 400.0f)) {
            layoutSettings_.killIconLocalPosition = { killIconPosition[0], killIconPosition[1] };
            ApplyLayout();
        }

        float killScorePosition[2]{ layoutSettings_.killScoreLocalPosition.x, layoutSettings_.killScoreLocalPosition.y };
        if (ImGui::DragFloat2("Kill Score", killScorePosition, 1.0f, -400.0f, 400.0f)) {
            layoutSettings_.killScoreLocalPosition = { killScorePosition[0], killScorePosition[1] };
            ApplyLayout();
        }

        float buildRowPosition[2]{ layoutSettings_.buildRowLocalPosition.x, layoutSettings_.buildRowLocalPosition.y };
        if (ImGui::DragFloat2("Build Row", buildRowPosition, 1.0f, -400.0f, 400.0f)) {
            layoutSettings_.buildRowLocalPosition = { buildRowPosition[0], buildRowPosition[1] };
            ApplyLayout();
        }

        if (ImGui::DragFloat("Build Spacing", &layoutSettings_.buildStepX, 1.0f, 32.0f, 240.0f)) {
            ApplyLayout();
        }

        float buildIconSize[2]{ layoutSettings_.buildIconSize.x, layoutSettings_.buildIconSize.y };
        if (ImGui::DragFloat2("Build Size", buildIconSize, 1.0f, 16.0f, 256.0f)) {
            layoutSettings_.buildIconSize = { buildIconSize[0], buildIconSize[1] };
            ApplyLayout();
        }

        if (ImGui::Button("Save Pause Layout")) {
            UILayoutIO::Save(kPauseLayoutPath, {
                { "statsPanelPosition", { layoutSettings_.statsPanelPosition.x, layoutSettings_.statsPanelPosition.y } },
                { "statsPanelSize", { layoutSettings_.statsPanelSize.x, layoutSettings_.statsPanelSize.y } },
                { "statsPanelAnchor", { static_cast<float>(static_cast<int>(layoutSettings_.statsPanelAnchor)) } },
                { "killIconLocalPosition", { layoutSettings_.killIconLocalPosition.x, layoutSettings_.killIconLocalPosition.y } },
                { "buildRowLocalPosition", { layoutSettings_.buildRowLocalPosition.x, layoutSettings_.buildRowLocalPosition.y } },
                { "buildStepX", { layoutSettings_.buildStepX } },
                { "buildIconSize", { layoutSettings_.buildIconSize.x, layoutSettings_.buildIconSize.y } },
                { "killScoreLocalPosition", { layoutSettings_.killScoreLocalPosition.x, layoutSettings_.killScoreLocalPosition.y } },
            });
        }
    }

    ImGui::End();
#endif
}

void GamePauseController::ApplyLayout() {
    statsPanel_->SetAnchor(layoutSettings_.statsPanelAnchor);
    statsPanel_->SetPosition(layoutSettings_.statsPanelPosition);
    statsPanel_->SetSize(layoutSettings_.statsPanelSize);

    killIconLabel_->SetPosition(layoutSettings_.killIconLocalPosition);

    buildNormalLabel_->SetPosition(layoutSettings_.buildRowLocalPosition);
    buildNormalLabel_->SetSize(layoutSettings_.buildIconSize);

    buildOrbitLabel_->SetPosition({ layoutSettings_.buildRowLocalPosition.x + layoutSettings_.buildStepX,
                                    layoutSettings_.buildRowLocalPosition.y });
    buildOrbitLabel_->SetSize(layoutSettings_.buildIconSize);

    buildDroneLabel_->SetPosition({ layoutSettings_.buildRowLocalPosition.x + layoutSettings_.buildStepX * 2.0f,
                                    layoutSettings_.buildRowLocalPosition.y });
    buildDroneLabel_->SetSize(layoutSettings_.buildIconSize);

    buildAttackLabel_->SetPosition({ layoutSettings_.buildRowLocalPosition.x + layoutSettings_.buildStepX * 3.0f,
                                     layoutSettings_.buildRowLocalPosition.y });
    buildAttackLabel_->SetSize(layoutSettings_.buildIconSize);
}

void GamePauseController::UpdateStats(const EnemyManager& enemyManager, const PlayerManager& playerManager) {
    const Vector2 panelWorld = statsPanel_->GetWorldPosition();
    killScore_->SetPosition({panelWorld.x + layoutSettings_.killScoreLocalPosition.x,
                             panelWorld.y + layoutSettings_.killScoreLocalPosition.y});
    killScore_->SetNumber(enemyManager.GetTotalKillCount());

    buildNormalLabel_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
    buildOrbitLabel_->SetColor({1.0f, 1.0f, 1.0f, playerManager.HasOrbitBullets() ? 1.0f : 0.25f});
    buildDroneLabel_->SetColor({1.0f, 1.0f, 1.0f, playerManager.HasDrone() ? 1.0f : 0.25f});
    buildAttackLabel_->SetColor({1.0f, 1.0f, 1.0f, playerManager.GetAttackPower() > 1 ? 1.0f : 0.4f});
}

void GamePauseController::UpdateVisibility() {
    const bool showStats = active_ && !guideActive_ && guideTransitionState_ == GuideTransitionState::None;
    statsPanel_->SetVisible(showStats);
    killIconLabel_->SetVisible(showStats);
    buildNormalLabel_->SetVisible(showStats);
    buildOrbitLabel_->SetVisible(showStats);
    buildDroneLabel_->SetVisible(showStats);
    buildAttackLabel_->SetVisible(showStats);
}

} // namespace DirectXGame
