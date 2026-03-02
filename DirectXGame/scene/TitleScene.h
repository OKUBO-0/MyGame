#pragma once

#include "IScene.h"
#include "../2d/CurtainTransition.h"
#include "../3d/SkyDome.h"
#include <KamataEngine.h>
#include <memory>

/// <summary>
/// タイトルシーンを管理するクラス。
/// 背景やUI、演出、BGMを制御し、ゲーム開始前の画面を構成する。
/// </summary>
class TitleScene : public IScene {
public:
    /// <summary>初期化処理（背景スプライトやUI、BGMの準備を行う）</summary>
    void Initialize() override;

    /// <summary>毎フレーム更新処理（入力判定や演出更新を行う）</summary>
    void Update() override;

    /// <summary>描画処理（背景、UI、演出を画面に描画する）</summary>
    void Draw() override;

    /// <summary>終了処理（リソース解放やシーン終了時の後処理を行う）</summary>
    void Finalize() override;

    /// <summary>シーンが終了状態かどうかを判定する</summary>
    bool IsFinished() const override { return finished_; }

private:
    KamataEngine::DirectXCommon* dxCommon_ = nullptr; ///< DirectX管理（外部から取得）
    KamataEngine::Input* input_ = nullptr;            ///< 入力管理（外部から取得）
    KamataEngine::Audio* audio_ = nullptr;            ///< オーディオ管理（外部から取得）

    std::unique_ptr<KamataEngine::Sprite> titleSprite_;      ///< タイトル文字スプライト
    std::unique_ptr<KamataEngine::Sprite> titleUISprite_;    ///< タイトルUIスプライト
    std::unique_ptr<KamataEngine::Sprite> cursorSprite_;
	std::unique_ptr<KamataEngine::Sprite> guideSprite_;      ///< ガイドUIスプライト

    CurtainTransition curtain_;
    bool curtainStarted_ = false;
    bool curtainOpening_ = true;
    bool finished_ = false;       ///< シーン終了フラグ

    uint32_t titleBGMHandle_ = 0; ///< タイトルBGMハンドル
    uint32_t selectSEHandle_ = 0; ///< 選択SEハンドル

    int32_t menuIndex_ = 0;

    bool guideActive_ = false;

    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Camera camera_;
    std::unique_ptr<KamataEngine::Model> playerModel_;
    std::unique_ptr<SkyDome> skyDome_;
};