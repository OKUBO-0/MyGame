#include "SkyDome.h"
using namespace KamataEngine;

void SkyDome::Initialize() {
    // カメラ初期化（背景専用カメラを準備）
    camera_.Initialize();

    // モデル生成（天球モデルを読み込み、背景の空を表現）
    skyModel_.reset(Model::CreateFromOBJ("skydome"));

    // ワールド変換初期化（位置・回転・スケールの基準を設定）
    worldTransform_.Initialize();
}

void SkyDome::Update() {
    // 背景なので特別な更新は不要
    // 必要に応じて回転や動きを加える場合はここに処理を追加
}

void SkyDome::Draw() {
    if (skyModel_) {
        skyModel_->Draw(worldTransform_, camera_);
    }
}