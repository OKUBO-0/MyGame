#include "SkyDome.h"

using namespace KamataEngine;

SkyDome::SkyDome() {
}

SkyDome::~SkyDome() {
    /// <summary>
    /// 動的に生成したスカイドームモデルを解放
    /// </summary>
    delete skyModel_;
}

void SkyDome::Initialize() {
    /// <summary>
    /// カメラを初期化（スカイドーム専用の描画用カメラ）
    /// </summary>
    camera_.Initialize();

    /// <summary>
    /// スカイドームモデルを読み込み
    /// skydome.obj は全天球型の背景モデルを想定
    /// </summary>
    skyModel_ = Model::CreateFromOBJ("skydome");

    /// <summary>
    /// ワールドトランスフォームを初期化
    /// スカイドームは原点に配置し、カメラを包み込むように描画される
    /// </summary>
    worldTransform_.Initialize();
}

void SkyDome::Update() {
    /// <summary>
    /// スカイドームは背景として固定されるため、特別な更新処理は不要
    /// 必要に応じて回転や動きを加える場合はここに処理を追加する
    /// </summary>
}

void SkyDome::Draw() {
    /// <summary>
    /// スカイドームモデルを描画
    /// ワールド座標と専用カメラを用いて背景として表示する
    /// </summary>
    skyModel_->Draw(worldTransform_, camera_);
}