#include "TitleUI.h"
using namespace KamataEngine;

TitleUI::TitleUI() {}

TitleUI::~TitleUI() {
    /// <summary>
    /// 動的に生成したモデルを解放
    /// </summary>
    delete playerModel_;
}

void TitleUI::Initialize() {
    /// <summary>
    /// タイトル演出用モデルを読み込み（octopus.obj）
    /// </summary>
    playerModel_ = Model::CreateFromOBJ("octopus");

    /// <summary>
    /// ワールドトランスフォーム初期化
    /// </summary>
    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
    worldTransform_.scale_ = { 2.0f, 2.0f, 2.0f };

    /// <summary>
    /// 初期回転（ここでは0で開始、演出で回転を加える）
    /// </summary>
    worldTransform_.rotation_ = { 0.0f, 0.0f, 0.0f };

    /// <summary>
    /// 専用カメラ初期化
    /// </summary>
    camera_.Initialize();
}

void TitleUI::Update() {
    /// <summary>
    /// ワールド行列を更新（位置・回転・スケールを反映）
    /// </summary>
    worldTransform_.UpdateMatrix();
}

void TitleUI::Draw() {
    /// <summary>
    /// モデルを描画
    /// </summary>
    playerModel_->Draw(worldTransform_, camera_);
}

void TitleUI::Finalize() {
    /// <summary>
    /// 特別な終了処理は不要（リソース解放はデストラクタで対応）
    /// </summary>
}

void TitleUI::SetPositionZ(float z) {
    /// <summary>
    /// Z座標を更新（奥行き方向の移動演出に利用）
    /// </summary>
    worldTransform_.translation_.z = z;
    worldTransform_.UpdateMatrix();
}

float TitleUI::GetPositionZ() const {
    /// <summary>
    /// 現在のZ座標を返す
    /// </summary>
    return worldTransform_.translation_.z;
}

void TitleUI::AddRotation(float angle) {
    /// <summary>
    /// 回転演出（Y軸とZ軸に回転を加える）
    /// </summary>
    worldTransform_.rotation_.z += angle;
    worldTransform_.rotation_.y += angle;
    worldTransform_.UpdateMatrix();
}