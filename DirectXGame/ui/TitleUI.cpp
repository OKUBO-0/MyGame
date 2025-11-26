#include "TitleUI.h"
using namespace KamataEngine;

TitleUI::TitleUI() {}
TitleUI::~TitleUI() {
    // 動的に生成したモデルを解放
    delete playerModel_;
}

void TitleUI::Initialize() {
    // タイトル演出用モデルを読み込み（octopus.obj）
    playerModel_ = Model::CreateFromOBJ("octopus");

    // ワールドトランスフォーム初期化
    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f }; // 初期位置は原点
    worldTransform_.scale_ = { 2.0f, 2.0f, 2.0f };       // 倍率を大きめに設定

    // 初期回転（ここでは0で開始、演出で回転を加える）
    worldTransform_.rotation_ = { 0.0f, 0.0f, 0.0f };

    // 専用カメラ初期化
    camera_.Initialize();
}

void TitleUI::Update() {
    // ワールド行列を更新（位置・回転・スケールを反映）
    worldTransform_.UpdateMatrix();
}

void TitleUI::Draw() {
    // モデルを描画
    playerModel_->Draw(worldTransform_, camera_);
}

void TitleUI::Finalize() {
    // 特別な終了処理は不要（リソース解放はデストラクタで対応）
}

void TitleUI::SetPositionZ(float z) {
    // Z座標を更新（奥行き方向の移動演出に利用）
    worldTransform_.translation_.z = z;
    worldTransform_.UpdateMatrix();
}

float TitleUI::GetPositionZ() const {
    // 現在のZ座標を返す
    return worldTransform_.translation_.z;
}

void TitleUI::AddRotation(float angle) {
    // 回転演出（Y軸とZ軸に回転を加える）
    worldTransform_.rotation_.z += angle;
    worldTransform_.rotation_.y += angle;
    worldTransform_.UpdateMatrix();
}