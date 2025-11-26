#include "SkyDome.h"

using namespace KamataEngine;

SkyDome::SkyDome() {
}

SkyDome::~SkyDome() {
    // 動的に生成したスカイドームモデルを解放
    delete skyModel_;
}

void SkyDome::Initialize() {
    // カメラを初期化（スカイドーム専用の描画用カメラ）
    camera_.Initialize();

    // スカイドームモデルを読み込み
    // skydome.obj は全天球型の背景モデルを想定
    skyModel_ = Model::CreateFromOBJ("skydome");

    // ワールドトランスフォームを初期化
    // スカイドームは基本的に原点に配置し、カメラを包み込むように描画される
    worldTransform_.Initialize();
}

void SkyDome::Update() {
    // スカイドームは背景として固定されるため、特別な更新処理は不要
    // 必要に応じて回転や動きを加える場合はここに処理を追加する
}

void SkyDome::Draw() {
    // スカイドームモデルを描画
    // ワールド座標と専用カメラを用いて背景として表示する
    skyModel_->Draw(worldTransform_, camera_);
}