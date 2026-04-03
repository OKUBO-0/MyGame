#include "ResultScene.h"
#include "../../core/InputBindings.h"
#include "../../ui/common/UILayoutIO.h"
#include <algorithm>
using namespace KamataEngine;

namespace DirectXGame {

namespace {

const char* kResultLayoutPath = "Resources/data/ui_layout_result.csv";

int32_t GetResultIncrement(int32_t currentValue, int32_t targetValue, int32_t divisor) {
    return (std::max<int32_t>)(1, (targetValue - currentValue) / divisor);
}

bool IsPointInRect(const Vector2& point, const Vector2& rectPosition, const Vector2& rectSize) {
    return point.x >= rectPosition.x && point.x <= rectPosition.x + rectSize.x &&
           point.y >= rectPosition.y && point.y <= rectPosition.y + rectSize.y;
}

int32_t GetHoveredResultMenuIndex(Input* input, const Vector2* hitboxPositions, const Vector2& hitboxSize) {
    if (!input) {
        return -1;
    }

    const Vector2 mousePosition = input->GetMousePosition();
    for (int32_t i = 0; i < 2; ++i) {
        if (IsPointInRect(mousePosition, hitboxPositions[i], hitboxSize)) {
            return i;
        }
    }

    return -1;
}

}

void ResultScene::Initialize() {
    // --- 各種シングルトン取得（描画・入力・音声） ---
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    selectSEHandle_ = audio_->LoadWave("audio/se/se_pause.wav");
    countupSEHandle_ = audio_->LoadWave("audio/se/se_pause.wav");

    // --- 背景スプライト生成（黒背景） ---
    uint32_t blackTex = TextureManager::Load("textures/color/black.png");
    backgroundSprite_ = std::unique_ptr<Sprite>(Sprite::Create(blackTex, { 0,0 }));

    // --- リザルトタイトル ---
    uint32_t resultTex = TextureManager::Load("ui/result/cc0/result_title.png");
    resultSprite_ = std::unique_ptr<Sprite>(Sprite::Create(resultTex, { 0,0 }));

    // --- フレーム ---
    uint32_t uiTex = TextureManager::Load("ui/result/cc0/panel_bg.png");
    resultUI_ = std::unique_ptr<Sprite>(Sprite::Create(uiTex, { 0,0 }));
    titleBannerSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/result/cc0/title_banner.png"), { 0,0 }));

    expLabelSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/result/cc0/label_exp.png"), { 0,0 }));
    levelLabelSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/result/cc0/label_level.png"), { 0,0 }));
    killLabelSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/result/cc0/label_kill.png"), { 0,0 }));
    timeLabelSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/result/cc0/label_time.png"), { 0,0 }));
    totalScoreLabelSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/result/cc0/label_total_score.png"), { 0,0 }));
    retryButtonSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/result/cc0/button_retry.png"), { 0,0 }));
    titleButtonSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/result/cc0/button_title.png"), { 0,0 }));
    retryLabelSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/result/cc0/menu_retry.png"), { 0,0 }));
    titleLabelSprite_ = std::unique_ptr<Sprite>(Sprite::Create(TextureManager::Load("ui/result/cc0/menu_title.png"), { 0,0 }));

    uint32_t cursorTex = TextureManager::Load("ui/result/cc0/cursor_arrow.png");
    cursorSprite_ = std::unique_ptr<Sprite>(Sprite::Create(cursorTex, { 0,0 }));

    // 経験値
    expUI_ = std::make_unique<Score>();
    expUI_->Initialize();
    expUI_->SetNumber(0);

    // レベル
    levelUI_ = std::make_unique<Score>();
    levelUI_->Initialize();
    levelUI_->SetNumber(sessionContext_ ? sessionContext_->resultData.finalLevel : 0);

    // キル数
    killUI_ = std::make_unique<Score>();
    killUI_->Initialize();
    killUI_->SetNumber(sessionContext_ ? sessionContext_->resultData.totalKillCount : 0);

    // 生存時間
    timeUI_ = std::make_unique<Score>();
    timeUI_->Initialize();
    timeUI_->SetNumber(sessionContext_ ? sessionContext_->resultData.survivalSeconds : 0);

    // 合計スコア
    totalScoreUI_ = std::make_unique<Score>();
    totalScoreUI_->Initialize();
    totalScoreUI_->SetNumber(sessionContext_ ? sessionContext_->resultData.totalScore : 0);

    // --- スコア演出用変数 ---
    currentExp_ = 0;
    targetExp_ = sessionContext_ ? sessionContext_->resultData.totalExp : 0;

    currentLevel_ = 0;
    targetLevel_ = sessionContext_ ? sessionContext_->resultData.finalLevel : 0;

    currentKill_ = 0;
    targetKill_ = sessionContext_ ? sessionContext_->resultData.totalKillCount : 0;

    currentTime_ = 0;
    targetTime_ = sessionContext_ ? sessionContext_->resultData.survivalSeconds : 0;

    currentTotalScore_ = 0;
    targetTotalScore_ = sessionContext_ ? sessionContext_->resultData.totalScore : 0;

    expUI_->SetNumber(0);
    levelUI_->SetNumber(0);
    killUI_->SetNumber(0);
    timeUI_->SetNumber(0);
    totalScoreUI_->SetNumber(0);

    // --- カーテン初期化 ---
    curtain_.Initialize();
    curtain_.StartOpen(20.0f);
    curtainOpening_ = true;
    pendingScene_ = Scene::Title;
    menuIndex_ = 0;
    navigationInputDevice_ = InputBindings::NavigationInputDevice::Mouse;
    {
        const auto layout = UILayoutIO::Load(kResultLayoutPath);
        if (const auto it = layout.find("backgroundPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.backgroundPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("backgroundSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.backgroundSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("panelPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.panelPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("panelSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.panelSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("titleBannerPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.titleBannerPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("titleBannerSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.titleBannerSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("titlePosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.titlePosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("titleSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.titleSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("expLabelPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.expLabelPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("expLabelSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.expLabelSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("levelLabelPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.levelLabelPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("levelLabelSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.levelLabelSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("killLabelPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.killLabelPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("killLabelSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.killLabelSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("timeLabelPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.timeLabelPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("timeLabelSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.timeLabelSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("totalScoreLabelPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.totalScoreLabelPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("totalScoreLabelSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.totalScoreLabelSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("expPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.expPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("levelPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.levelPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("killPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.killPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("timePosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.timePosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("totalScorePosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.totalScorePosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("retryLabelPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.retryLabelPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("retryLabelSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.retryLabelSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("titleMenuLabelPosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.titleMenuLabelPosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("titleMenuLabelSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.titleMenuLabelSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("cursorBasePosition"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.cursorBasePosition = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("cursorSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.cursorSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("cursorStepX"); it != layout.end() && !it->second.empty()) {
            layoutSettings_.cursorStepX = it->second[0];
        }
        if (const auto it = layout.find("menuHitbox0"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.menuHitboxPositions[0] = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("menuHitbox1"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.menuHitboxPositions[1] = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("menuHitboxSize"); it != layout.end() && it->second.size() >= 2) {
            layoutSettings_.menuHitboxSize = { it->second[0], it->second[1] };
        }
        if (const auto it = layout.find("scoreScale"); it != layout.end() && !it->second.empty()) {
            layoutSettings_.scoreScale = it->second[0];
        }
    }
    ApplyLayout();
}

void ResultScene::Update(float deltaTime) {
    // --- カーテン更新（常に先頭で処理） ---
    curtain_.Update(deltaTime);
    if (countupSECooldown_ > 0) {
        --countupSECooldown_;
    }

    if (curtainOutStarted_) {
        if (curtain_.IsFinished()) {
            finished_ = true;
        }
        DrawDebugUI();
        return;
    }

    if (curtainOpening_) {
        if (curtain_.GetState() == CurtainTransition::State::kNone) {
            curtainOpening_ = false;
        }
        DrawDebugUI();
        return;
    }

    // --- スコア加算演出（徐々に最終スコアまで増加させる） ---
    if (currentExp_ < targetExp_) {
        currentExp_ += GetResultIncrement(currentExp_, targetExp_, 12);
        currentExp_ = (std::min)(currentExp_, targetExp_);
        expUI_->SetNumber(currentExp_);
        if (countupSEHandle_ != 0 && countupSECooldown_ == 0) {
            audio_->PlayWave(countupSEHandle_, false, 0.25f);
            countupSECooldown_ = 6;
        }
    }

    // --- レベル加算演出 ---
    if (currentLevel_ < targetLevel_) {
        currentLevel_ += GetResultIncrement(currentLevel_, targetLevel_, 8);
        currentLevel_ = (std::min)(currentLevel_, targetLevel_);
        levelUI_->SetNumber(currentLevel_);
        if (countupSEHandle_ != 0 && countupSECooldown_ == 0) {
            audio_->PlayWave(countupSEHandle_, false, 0.25f);
            countupSECooldown_ = 6;
        }
    }

    // --- キル数加算演出 ---
    if (currentKill_ < targetKill_) {
        currentKill_ += GetResultIncrement(currentKill_, targetKill_, 8);
        currentKill_ = (std::min)(currentKill_, targetKill_);
        killUI_->SetNumber(currentKill_);
        if (countupSEHandle_ != 0 && countupSECooldown_ == 0) {
            audio_->PlayWave(countupSEHandle_, false, 0.25f);
            countupSECooldown_ = 6;
        }
    }

    // --- 生存時間加算演出 ---
    if (currentTime_ < targetTime_) {
        currentTime_ += GetResultIncrement(currentTime_, targetTime_, 8);
        currentTime_ = (std::min)(currentTime_, targetTime_);
        timeUI_->SetNumber(currentTime_);
        if (countupSEHandle_ != 0 && countupSECooldown_ == 0) {
            audio_->PlayWave(countupSEHandle_, false, 0.25f);
            countupSECooldown_ = 6;
        }
    }

    // --- 合計スコア加算演出 ---
    if (currentTotalScore_ < targetTotalScore_) {
        currentTotalScore_ += GetResultIncrement(currentTotalScore_, targetTotalScore_, 10);
        currentTotalScore_ = (std::min)(currentTotalScore_, targetTotalScore_);
        totalScoreUI_->SetNumber(currentTotalScore_);
        if (countupSEHandle_ != 0 && countupSECooldown_ == 0) {
            audio_->PlayWave(countupSEHandle_, false, 0.25f);
            countupSECooldown_ = 6;
        }
    }

    expUI_->Update();
    levelUI_->Update();
    killUI_->Update();
    timeUI_->Update();
    totalScoreUI_->Update();

    const bool canSkipCountUp =
        currentExp_ < targetExp_ ||
        currentLevel_ < targetLevel_ ||
        currentKill_ < targetKill_ ||
        currentTime_ < targetTime_ ||
        currentTotalScore_ < targetTotalScore_;

    const int32_t hoveredMenuIndex =
        GetHoveredResultMenuIndex(input_, layoutSettings_.menuHitboxPositions, layoutSettings_.menuHitboxSize);
    const bool mouseNavigationTriggered = hoveredMenuIndex >= 0 && InputBindings::HasMouseNavigationInput(input_);

    if (InputBindings::IsGamepadMenuUpTriggered(input_) || InputBindings::IsGamepadMenuDownTriggered(input_) ||
        InputBindings::IsGamepadConfirmTriggered(input_)) {
        navigationInputDevice_ = InputBindings::NavigationInputDevice::Gamepad;
    } else if (mouseNavigationTriggered) {
        navigationInputDevice_ = InputBindings::NavigationInputDevice::Mouse;
    }

    if (InputBindings::IsUiConfirmTriggered(input_) && canSkipCountUp) {
        currentExp_ = targetExp_;
        currentLevel_ = targetLevel_;
        currentKill_ = targetKill_;
        currentTime_ = targetTime_;
        currentTotalScore_ = targetTotalScore_;
        expUI_->SetNumber(currentExp_);
        levelUI_->SetNumber(currentLevel_);
        killUI_->SetNumber(currentKill_);
        timeUI_->SetNumber(currentTime_);
        totalScoreUI_->SetNumber(currentTotalScore_);
        DrawDebugUI();
        return;
    }

    int32_t previousMenuIndex = menuIndex_;
    if (navigationInputDevice_ == InputBindings::NavigationInputDevice::Mouse) {
        if (hoveredMenuIndex >= 0) {
            menuIndex_ = hoveredMenuIndex;
        }
    } else if (navigationInputDevice_ == InputBindings::NavigationInputDevice::Gamepad) {
        if (InputBindings::IsGamepadMenuUpTriggered(input_)) {
            menuIndex_ = (std::max)(0, menuIndex_ - 1);
        } else if (InputBindings::IsGamepadMenuDownTriggered(input_)) {
            menuIndex_ = (std::min)(1, menuIndex_ + 1);
        }
    }

    if (menuIndex_ != previousMenuIndex && selectSEHandle_ != 0) {
        audio_->PlayWave(selectSEHandle_, false, 1.0f);
    }

    bool confirmTriggered = false;
    switch (navigationInputDevice_) {
    case InputBindings::NavigationInputDevice::Mouse:
        confirmTriggered = hoveredMenuIndex >= 0 && InputBindings::IsMouseConfirmTriggered(input_);
        break;
    case InputBindings::NavigationInputDevice::Gamepad:
        confirmTriggered = InputBindings::IsGamepadConfirmTriggered(input_);
        break;
    case InputBindings::NavigationInputDevice::None:
    case InputBindings::NavigationInputDevice::Keyboard:
        break;
    }

    if (confirmTriggered && curtain_.GetState() == CurtainTransition::State::kNone) {
        if (selectSEHandle_ != 0) {
            audio_->PlayWave(selectSEHandle_, false, 1.0f);
        }
        pendingScene_ = menuIndex_ == 0 ? Scene::Game : Scene::Title;
        curtain_.StartClose();
        curtainOutStarted_ = true;
        SetSceneNo(pendingScene_);
    }

    DrawDebugUI();
}

void ResultScene::Draw() {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // --- スプライト描画開始 ---
    Sprite::PreDraw(dxCommon->GetCommandList());

    // --- 背景・リザルト画面・UIを描画 ---
    backgroundSprite_->Draw();
    resultUI_->Draw();
    titleBannerSprite_->Draw();
    resultSprite_->Draw();
    expLabelSprite_->Draw();
    levelLabelSprite_->Draw();
    killLabelSprite_->Draw();
    timeLabelSprite_->Draw();
    totalScoreLabelSprite_->Draw();

    // --- スコア描画（加算演出で更新された値を表示） ---
    if (expUI_) {
        expUI_->Draw();
    }
    if (levelUI_) {
        levelUI_->Draw();
	}
    if (killUI_) {
		killUI_->Draw();
    }
    if (timeUI_) {
        timeUI_->Draw();
    }
    if (totalScoreUI_) {
        totalScoreUI_->Draw();
    }
    retryButtonSprite_->Draw();
    titleButtonSprite_->Draw();
    retryLabelSprite_->Draw();
    titleLabelSprite_->Draw();
    if (cursorSprite_) {
        cursorSprite_->Draw();
    }

    // --- カーテン描画（シーン遷移演出） ---
    curtain_.Draw();

    // --- スプライト描画終了 ---
    Sprite::PostDraw();
}

void ResultScene::Finalize() {
    // --- 特別な終了処理は不要（リソース解放はデストラクタで対応） ---
}

void ResultScene::ApplyLayout() {
    backgroundSprite_->SetPosition(layoutSettings_.backgroundPosition);
    backgroundSprite_->SetSize(layoutSettings_.backgroundSize);
    resultUI_->SetPosition(layoutSettings_.panelPosition);
    resultUI_->SetSize(layoutSettings_.panelSize);
    titleBannerSprite_->SetPosition(layoutSettings_.titleBannerPosition);
    titleBannerSprite_->SetSize(layoutSettings_.titleBannerSize);
    resultSprite_->SetPosition(layoutSettings_.titlePosition);
    resultSprite_->SetSize(layoutSettings_.titleSize);
    expLabelSprite_->SetPosition(layoutSettings_.expLabelPosition);
    expLabelSprite_->SetSize(layoutSettings_.expLabelSize);
    levelLabelSprite_->SetPosition(layoutSettings_.levelLabelPosition);
    levelLabelSprite_->SetSize(layoutSettings_.levelLabelSize);
    killLabelSprite_->SetPosition(layoutSettings_.killLabelPosition);
    killLabelSprite_->SetSize(layoutSettings_.killLabelSize);
    timeLabelSprite_->SetPosition(layoutSettings_.timeLabelPosition);
    timeLabelSprite_->SetSize(layoutSettings_.timeLabelSize);
    totalScoreLabelSprite_->SetPosition(layoutSettings_.totalScoreLabelPosition);
    totalScoreLabelSprite_->SetSize(layoutSettings_.totalScoreLabelSize);

    expUI_->SetPosition(layoutSettings_.expPosition);
    expUI_->SetScale(layoutSettings_.scoreScale);
    levelUI_->SetPosition(layoutSettings_.levelPosition);
    levelUI_->SetScale(layoutSettings_.scoreScale);
    killUI_->SetPosition(layoutSettings_.killPosition);
    killUI_->SetScale(layoutSettings_.scoreScale);
    timeUI_->SetPosition(layoutSettings_.timePosition);
    timeUI_->SetScale(layoutSettings_.scoreScale);
    totalScoreUI_->SetPosition(layoutSettings_.totalScorePosition);
    totalScoreUI_->SetScale(layoutSettings_.scoreScale);
    retryButtonSprite_->SetPosition(layoutSettings_.retryLabelPosition);
    retryButtonSprite_->SetSize(layoutSettings_.retryLabelSize);
    titleButtonSprite_->SetPosition(layoutSettings_.titleMenuLabelPosition);
    titleButtonSprite_->SetSize(layoutSettings_.titleMenuLabelSize);
    retryLabelSprite_->SetPosition(layoutSettings_.retryLabelPosition);
    retryLabelSprite_->SetSize(layoutSettings_.retryLabelSize);
    titleLabelSprite_->SetPosition(layoutSettings_.titleMenuLabelPosition);
    titleLabelSprite_->SetSize(layoutSettings_.titleMenuLabelSize);
    cursorSprite_->SetPosition({
        layoutSettings_.cursorBasePosition.x + layoutSettings_.cursorStepX * static_cast<float>(menuIndex_),
        layoutSettings_.cursorBasePosition.y
    });
    cursorSprite_->SetSize(layoutSettings_.cursorSize);
}

void ResultScene::DrawDebugUI() {
#ifdef _DEBUG
    if (!ImGui::Begin("UI Debug")) {
        ImGui::End();
        return;
    }

    if (ImGui::CollapsingHeader("Result", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Enable Result Debug", &layoutSettings_.debugEnabled);
        if (layoutSettings_.debugEnabled) {
            float panelPosition[2]{ layoutSettings_.panelPosition.x, layoutSettings_.panelPosition.y };
            if (ImGui::DragFloat2("Panel Position", panelPosition, 1.0f, -400.0f, 1280.0f)) {
                layoutSettings_.panelPosition = { panelPosition[0], panelPosition[1] };
                ApplyLayout();
            }

            float titlePosition[2]{ layoutSettings_.titlePosition.x, layoutSettings_.titlePosition.y };
            if (ImGui::DragFloat2("Title Position", titlePosition, 1.0f, -400.0f, 1280.0f)) {
                layoutSettings_.titlePosition = { titlePosition[0], titlePosition[1] };
                ApplyLayout();
            }

            float titleBannerPosition[2]{ layoutSettings_.titleBannerPosition.x, layoutSettings_.titleBannerPosition.y };
            if (ImGui::DragFloat2("Title Banner Pos", titleBannerPosition, 1.0f, -400.0f, 1280.0f)) {
                layoutSettings_.titleBannerPosition = { titleBannerPosition[0], titleBannerPosition[1] };
                ApplyLayout();
            }

            float titleBannerSize[2]{ layoutSettings_.titleBannerSize.x, layoutSettings_.titleBannerSize.y };
            if (ImGui::DragFloat2("Title Banner Size", titleBannerSize, 1.0f, 32.0f, 640.0f)) {
                layoutSettings_.titleBannerSize = { titleBannerSize[0], titleBannerSize[1] };
                ApplyLayout();
            }

            float expPosition[2]{ layoutSettings_.expPosition.x, layoutSettings_.expPosition.y };
            if (ImGui::DragFloat2("EXP Position", expPosition, 1.0f, -400.0f, 1280.0f)) {
                layoutSettings_.expPosition = { expPosition[0], expPosition[1] };
                ApplyLayout();
            }

            float levelPosition[2]{ layoutSettings_.levelPosition.x, layoutSettings_.levelPosition.y };
            if (ImGui::DragFloat2("Level Position", levelPosition, 1.0f, -400.0f, 1280.0f)) {
                layoutSettings_.levelPosition = { levelPosition[0], levelPosition[1] };
                ApplyLayout();
            }

            float killPosition[2]{ layoutSettings_.killPosition.x, layoutSettings_.killPosition.y };
            if (ImGui::DragFloat2("Kill Position", killPosition, 1.0f, -400.0f, 1280.0f)) {
                layoutSettings_.killPosition = { killPosition[0], killPosition[1] };
                ApplyLayout();
            }

            float timePosition[2]{ layoutSettings_.timePosition.x, layoutSettings_.timePosition.y };
            if (ImGui::DragFloat2("Time Position", timePosition, 1.0f, -400.0f, 1280.0f)) {
                layoutSettings_.timePosition = { timePosition[0], timePosition[1] };
                ApplyLayout();
            }

            float totalScorePosition[2]{ layoutSettings_.totalScorePosition.x, layoutSettings_.totalScorePosition.y };
            if (ImGui::DragFloat2("Total Score Position", totalScorePosition, 1.0f, -400.0f, 1280.0f)) {
                layoutSettings_.totalScorePosition = { totalScorePosition[0], totalScorePosition[1] };
                ApplyLayout();
            }

            float cursorBase[2]{ layoutSettings_.cursorBasePosition.x, layoutSettings_.cursorBasePosition.y };
            if (ImGui::DragFloat2("Cursor Base", cursorBase, 1.0f, -400.0f, 1280.0f)) {
                layoutSettings_.cursorBasePosition = { cursorBase[0], cursorBase[1] };
                ApplyLayout();
            }

            float cursorSize[2]{ layoutSettings_.cursorSize.x, layoutSettings_.cursorSize.y };
            if (ImGui::DragFloat2("Cursor Size", cursorSize, 1.0f, 64.0f, 1280.0f)) {
                layoutSettings_.cursorSize = { cursorSize[0], cursorSize[1] };
                ApplyLayout();
            }

            if (ImGui::DragFloat("Cursor Step X", &layoutSettings_.cursorStepX, 1.0f, 8.0f, 320.0f)) {
                ApplyLayout();
            }

            float menuHitbox0[2]{ layoutSettings_.menuHitboxPositions[0].x, layoutSettings_.menuHitboxPositions[0].y };
            if (ImGui::DragFloat2("Menu Hitbox 0", menuHitbox0, 1.0f, -400.0f, 1280.0f)) {
                layoutSettings_.menuHitboxPositions[0] = { menuHitbox0[0], menuHitbox0[1] };
            }

            float menuHitbox1[2]{ layoutSettings_.menuHitboxPositions[1].x, layoutSettings_.menuHitboxPositions[1].y };
            if (ImGui::DragFloat2("Menu Hitbox 1", menuHitbox1, 1.0f, -400.0f, 1280.0f)) {
                layoutSettings_.menuHitboxPositions[1] = { menuHitbox1[0], menuHitbox1[1] };
            }

            float menuHitboxSize[2]{ layoutSettings_.menuHitboxSize.x, layoutSettings_.menuHitboxSize.y };
            if (ImGui::DragFloat2("Menu Hitbox Size", menuHitboxSize, 1.0f, 16.0f, 640.0f)) {
                layoutSettings_.menuHitboxSize = { menuHitboxSize[0], menuHitboxSize[1] };
            }

            if (ImGui::DragFloat("Score Scale", &layoutSettings_.scoreScale, 0.05f, 0.5f, 6.0f)) {
                ApplyLayout();
            }

            if (ImGui::Button("Save Result Layout")) {
                UILayoutIO::Save(kResultLayoutPath, {
                    { "backgroundPosition", { layoutSettings_.backgroundPosition.x, layoutSettings_.backgroundPosition.y } },
                    { "backgroundSize", { layoutSettings_.backgroundSize.x, layoutSettings_.backgroundSize.y } },
                    { "panelPosition", { layoutSettings_.panelPosition.x, layoutSettings_.panelPosition.y } },
                    { "panelSize", { layoutSettings_.panelSize.x, layoutSettings_.panelSize.y } },
                    { "titleBannerPosition", { layoutSettings_.titleBannerPosition.x, layoutSettings_.titleBannerPosition.y } },
                    { "titleBannerSize", { layoutSettings_.titleBannerSize.x, layoutSettings_.titleBannerSize.y } },
                    { "titlePosition", { layoutSettings_.titlePosition.x, layoutSettings_.titlePosition.y } },
                    { "titleSize", { layoutSettings_.titleSize.x, layoutSettings_.titleSize.y } },
                    { "expLabelPosition", { layoutSettings_.expLabelPosition.x, layoutSettings_.expLabelPosition.y } },
                    { "expLabelSize", { layoutSettings_.expLabelSize.x, layoutSettings_.expLabelSize.y } },
                    { "levelLabelPosition", { layoutSettings_.levelLabelPosition.x, layoutSettings_.levelLabelPosition.y } },
                    { "levelLabelSize", { layoutSettings_.levelLabelSize.x, layoutSettings_.levelLabelSize.y } },
                    { "killLabelPosition", { layoutSettings_.killLabelPosition.x, layoutSettings_.killLabelPosition.y } },
                    { "killLabelSize", { layoutSettings_.killLabelSize.x, layoutSettings_.killLabelSize.y } },
                    { "timeLabelPosition", { layoutSettings_.timeLabelPosition.x, layoutSettings_.timeLabelPosition.y } },
                    { "timeLabelSize", { layoutSettings_.timeLabelSize.x, layoutSettings_.timeLabelSize.y } },
                    { "totalScoreLabelPosition", { layoutSettings_.totalScoreLabelPosition.x, layoutSettings_.totalScoreLabelPosition.y } },
                    { "totalScoreLabelSize", { layoutSettings_.totalScoreLabelSize.x, layoutSettings_.totalScoreLabelSize.y } },
                    { "expPosition", { layoutSettings_.expPosition.x, layoutSettings_.expPosition.y } },
                    { "levelPosition", { layoutSettings_.levelPosition.x, layoutSettings_.levelPosition.y } },
                    { "killPosition", { layoutSettings_.killPosition.x, layoutSettings_.killPosition.y } },
                    { "timePosition", { layoutSettings_.timePosition.x, layoutSettings_.timePosition.y } },
                    { "totalScorePosition", { layoutSettings_.totalScorePosition.x, layoutSettings_.totalScorePosition.y } },
                    { "retryLabelPosition", { layoutSettings_.retryLabelPosition.x, layoutSettings_.retryLabelPosition.y } },
                    { "retryLabelSize", { layoutSettings_.retryLabelSize.x, layoutSettings_.retryLabelSize.y } },
                    { "titleMenuLabelPosition", { layoutSettings_.titleMenuLabelPosition.x, layoutSettings_.titleMenuLabelPosition.y } },
                    { "titleMenuLabelSize", { layoutSettings_.titleMenuLabelSize.x, layoutSettings_.titleMenuLabelSize.y } },
                    { "cursorBasePosition", { layoutSettings_.cursorBasePosition.x, layoutSettings_.cursorBasePosition.y } },
                    { "cursorSize", { layoutSettings_.cursorSize.x, layoutSettings_.cursorSize.y } },
                    { "cursorStepX", { layoutSettings_.cursorStepX } },
                    { "menuHitbox0", { layoutSettings_.menuHitboxPositions[0].x, layoutSettings_.menuHitboxPositions[0].y } },
                    { "menuHitbox1", { layoutSettings_.menuHitboxPositions[1].x, layoutSettings_.menuHitboxPositions[1].y } },
                    { "menuHitboxSize", { layoutSettings_.menuHitboxSize.x, layoutSettings_.menuHitboxSize.y } },
                    { "scoreScale", { layoutSettings_.scoreScale } },
                });
            }
        }
    }

    ImGui::End();
#endif
}

} // namespace DirectXGame
