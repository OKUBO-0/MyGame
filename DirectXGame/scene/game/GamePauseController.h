#pragma once

#include "../../ui/hud/MiniMap.h"
#include "../../ui/hud/Score.h"
#include <cstdint>
#include <memory>

namespace KamataEngine {
class Audio;
class DirectXCommon;
class Input;
class Sprite;
}

namespace DirectXGame {

class EnemyManager;
class Player;
class PlayerManager;

/// <summary>
/// ポーズ画面の入力と表示を管理するクラス。
/// ポーズメニュー、ガイド表示、ミニマップ表示、リザルト遷移要求を制御する。
/// </summary>
class GamePauseController {
public:
    /// <summary>
    /// 初期化処理
    /// 目的: ポーズUIとミニマップを生成して初期状態を整える。
    /// 引数: なし
    /// 戻り値: なし
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新処理
    /// 目的: ポーズ切替、メニュー操作、ガイド表示、遷移要求を更新する。
    /// 引数: player - ミニマップ更新に使うプレイヤー
    /// 引数: enemyManager - ミニマップ更新に使う敵管理
    /// 引数: input - 入力取得に使う入力管理
    /// 引数: audio - 効果音再生に使う音声管理
    /// 引数: toggleSEHandle - ポーズ開閉に使うSEハンドル
    /// 戻り値: true ならポーズ中としてゲーム更新を止める
    /// </summary>
    bool Update(Player* player, const EnemyManager& enemyManager, const PlayerManager& playerManager,
                KamataEngine::Input* input, KamataEngine::Audio* audio, uint32_t toggleSEHandle);

    /// <summary>
    /// 描画処理
    /// 目的: ポーズオーバーレイ、ガイド、カーソル、ミニマップを描画する。
    /// 引数: dxCommon - スプライト描画に使う DirectX 共通オブジェクト
    /// 戻り値: なし
    /// </summary>
    void Draw() const;

    /// <summary>
    /// 状態リセット
    /// 目的: 次回利用に備えてポーズ状態を初期値へ戻す。
    /// 引数: なし
    /// 戻り値: なし
    /// </summary>
    void Reset();

    /// <summary>
    /// ポーズ有効判定
    /// 目的: 現在ポーズ画面が有効かを返す。
    /// 引数: なし
    /// 戻り値: true ならポーズ中
    /// </summary>
    bool IsActive() const;

    /// <summary>
    /// ガイド表示判定
    /// 目的: ガイドUIが表示中かを返す。
    /// 引数: なし
    /// 戻り値: true ならガイド表示中
    /// </summary>
    bool IsGuideActive() const;

    /// <summary>
    /// リザルト遷移要求判定
    /// 目的: Finish 選択が行われたかを返す。
    /// 引数: なし
    /// 戻り値: true ならリザルトへ遷移すべき
    /// </summary>
    bool ShouldGoResult() const;

private:
    bool active_ = false;
    KamataEngine::Audio* audio_ = nullptr;

    std::unique_ptr<KamataEngine::Sprite> pauseOverlay_;
    std::unique_ptr<KamataEngine::Sprite> guideSprite_;
    std::unique_ptr<KamataEngine::Sprite> cursorSprite_;
    std::unique_ptr<MiniMap> miniMap_;
    std::unique_ptr<KamataEngine::Sprite> statsPanel_;
    std::unique_ptr<KamataEngine::Sprite> killIconSprite_;
    std::unique_ptr<KamataEngine::Sprite> buildNormalSprite_;
    std::unique_ptr<KamataEngine::Sprite> buildOrbitSprite_;
    std::unique_ptr<KamataEngine::Sprite> buildDroneSprite_;
    std::unique_ptr<KamataEngine::Sprite> buildAttackSprite_;
    std::unique_ptr<Score> killScore_;

    uint32_t selectSEHandle_ = 0;
    uint32_t decideSEHandle_ = 0;

    int32_t menuIndex_ = 0;
    bool guideActive_ = false;
    bool goResult_ = false;

    void UpdateStats(const EnemyManager& enemyManager, const PlayerManager& playerManager);
};

} // namespace DirectXGame
