#pragma once
#include "../core/IScene.h"
#include "../../core/InputBindings.h"
#include "../../effects/CurtainTransition.h"
#include "../../ui/hud/Score.h"
#include <KamataEngine.h>
#include <cstdint>
#include <memory>

namespace DirectXGame {

/// <summary>
/// リザルトシーンを管理するクラス。
/// スコア表示や演出を行い、ゲーム終了後の画面を制御する。
/// </summary>
class ResultScene : public IScene {
public:
    explicit ResultScene(std::shared_ptr<GameSessionContext> sessionContext)
        : IScene(std::move(sessionContext)) {}

    /// <summary>初期化処理（背景やUI、スコア表示の準備を行う）</summary>
    void Initialize() override;

    /// <summary>毎フレーム更新処理（演出やスコアのアニメーションを更新する）</summary>
    void Update(float deltaTime) override;

    /// <summary>描画処理（背景、UI、スコアを画面に描画する）</summary>
    void Draw() override;

    /// <summary>終了処理（リソース解放やシーン終了時の後処理を行う）</summary>
    void Finalize() override;

    /// <summary>シーンが終了状態かどうかを判定する</summary>
    bool IsFinished() const override { return finished_; }

private:
    struct LayoutSettings {
        KamataEngine::Vector2 backgroundPosition{ 0.0f, 0.0f };
        KamataEngine::Vector2 backgroundSize{ 1280.0f, 720.0f };
        KamataEngine::Vector2 panelPosition{ 80.0f, 40.0f };
        KamataEngine::Vector2 panelSize{ 1120.0f, 620.0f };
        KamataEngine::Vector2 titleBannerPosition{ 492.0f, 32.0f };
        KamataEngine::Vector2 titleBannerSize{ 296.0f, 88.0f };
        KamataEngine::Vector2 titlePosition{ 500.0f, 42.0f };
        KamataEngine::Vector2 titleSize{ 280.0f, 72.0f };
        KamataEngine::Vector2 expLabelPosition{ 128.0f, 186.0f };
        KamataEngine::Vector2 expLabelSize{ 380.0f, 70.0f };
        KamataEngine::Vector2 levelLabelPosition{ 128.0f, 282.0f };
        KamataEngine::Vector2 levelLabelSize{ 220.0f, 70.0f };
        KamataEngine::Vector2 killLabelPosition{ 128.0f, 378.0f };
        KamataEngine::Vector2 killLabelSize{ 320.0f, 70.0f };
        KamataEngine::Vector2 timeLabelPosition{ 128.0f, 474.0f };
        KamataEngine::Vector2 timeLabelSize{ 240.0f, 70.0f };
        KamataEngine::Vector2 totalScoreLabelPosition{ 440.0f, 548.0f };
        KamataEngine::Vector2 totalScoreLabelSize{ 360.0f, 80.0f };
        KamataEngine::Vector2 expPosition{ 720.0f, 200.0f };
        KamataEngine::Vector2 levelPosition{ 720.0f, 296.0f };
        KamataEngine::Vector2 killPosition{ 720.0f, 392.0f };
        KamataEngine::Vector2 timePosition{ 720.0f, 488.0f };
        KamataEngine::Vector2 totalScorePosition{ 820.0f, 558.0f };
        KamataEngine::Vector2 retryLabelPosition{ 470.0f, 618.0f };
        KamataEngine::Vector2 retryLabelSize{ 220.0f, 70.0f };
        KamataEngine::Vector2 titleMenuLabelPosition{ 700.0f, 618.0f };
        KamataEngine::Vector2 titleMenuLabelSize{ 220.0f, 70.0f };
        KamataEngine::Vector2 cursorBasePosition{ 400.0f, 618.0f };
        KamataEngine::Vector2 cursorSize{ 96.0f, 54.0f };
        float cursorStepX = 230.0f;
        KamataEngine::Vector2 menuHitboxPositions[2]{
            { 470.0f, 618.0f },
            { 700.0f, 618.0f },
        };
        KamataEngine::Vector2 menuHitboxSize{ 220.0f, 70.0f };
        float scoreScale = 2.0f;
        bool debugEnabled = false;
    };

    void ApplyLayout();
    void DrawDebugUI();

    KamataEngine::DirectXCommon* dxCommon_ = nullptr; ///< DirectX管理（外部から取得）
    KamataEngine::Input* input_ = nullptr;            ///< 入力管理（外部から取得）
    KamataEngine::Audio* audio_ = nullptr;            ///< オーディオ管理（外部から取得）

    std::unique_ptr<KamataEngine::Sprite> backgroundSprite_; ///< 背景スプライト
    std::unique_ptr<KamataEngine::Sprite> resultSprite_;     ///< リザルト文字スプライト
    std::unique_ptr<KamataEngine::Sprite> resultUI_;         ///< リザルトUIスプライト
    std::unique_ptr<KamataEngine::Sprite> titleBannerSprite_;
    std::unique_ptr<KamataEngine::Sprite> expLabelSprite_;
    std::unique_ptr<KamataEngine::Sprite> levelLabelSprite_;
    std::unique_ptr<KamataEngine::Sprite> killLabelSprite_;
    std::unique_ptr<KamataEngine::Sprite> timeLabelSprite_;
    std::unique_ptr<KamataEngine::Sprite> totalScoreLabelSprite_;
    std::unique_ptr<KamataEngine::Sprite> retryButtonSprite_;
    std::unique_ptr<KamataEngine::Sprite> titleButtonSprite_;
    std::unique_ptr<KamataEngine::Sprite> retryLabelSprite_;
    std::unique_ptr<KamataEngine::Sprite> titleLabelSprite_;
    std::unique_ptr<KamataEngine::Sprite> cursorSprite_;     ///< メニューカーソル

    CurtainTransition curtain_;   ///< カーテン演出
    bool curtainOutStarted_ = false;
    bool curtainOpening_ = true;
    bool finished_ = false;       ///< シーン終了フラグ
    Scene pendingScene_ = Scene::Title;

    std::unique_ptr<Score> expUI_;   ///< 経験値表示UI
    std::unique_ptr<Score> levelUI_; ///< レベル表示UI
    std::unique_ptr<Score> killUI_;  ///< 撃破数表示UI
    std::unique_ptr<Score> timeUI_;  ///< 生存時間表示UI
    std::unique_ptr<Score> totalScoreUI_; ///< 合計スコア表示UI

    int32_t currentExp_ = 0;       ///< 現在のスコア
    int32_t targetExp_ = 0;      ///< 目標スコア

    int32_t currentLevel_ = 0;    ///< 表示中レベル
    int32_t targetLevel_ = 0;     ///< 最終レベル

    int32_t currentKill_ = 0;     ///< 表示中撃破数
    int32_t targetKill_ = 0;      ///< 最終撃破数

    int32_t currentTime_ = 0;     ///< 表示中生存秒数
    int32_t targetTime_ = 0;      ///< 最終生存秒数

    int32_t currentTotalScore_ = 0; ///< 表示中合計スコア
    int32_t targetTotalScore_ = 0;  ///< 最終合計スコア

    uint32_t selectSEHandle_ = 0; ///< 選択SEハンドル
    uint32_t countupSEHandle_ = 0;
    int32_t countupSECooldown_ = 0;
    int32_t menuIndex_ = 0;
    InputBindings::NavigationInputDevice navigationInputDevice_ = InputBindings::NavigationInputDevice::Mouse;
    LayoutSettings layoutSettings_{};
};

} // namespace DirectXGame
