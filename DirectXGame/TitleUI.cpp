#include "TitleUI.h"
using namespace KamataEngine;

TitleUI::TitleUI() {}
TitleUI::~TitleUI() {
    delete playerModel_;
}

void TitleUI::Initialize() {
    playerModel_ = Model::CreateFromOBJ("octopus");

    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
    worldTransform_.scale_ = { 2.0f, 2.0f, 2.0f };

    // 初期回転をX=90°, Z=180°に設定（ラジアンではなく角度単位で扱う場合）
    worldTransform_.rotation_ = { 100.0f, 0.0f, 0.0f }; // {X, Y, Z}

    camera_.Initialize();
}

void TitleUI::Update() {
    worldTransform_.UpdateMatrix();
}

void TitleUI::Draw() {
    playerModel_->Draw(worldTransform_, camera_);
}

void TitleUI::Finalize() {}

void TitleUI::SetPositionZ(float z) {
    worldTransform_.translation_.z = z;
    worldTransform_.UpdateMatrix();
}

float TitleUI::GetPositionZ() const {
    return worldTransform_.translation_.z;
}

// --- Y軸回転追加 ---
void TitleUI::AddRotationY(float angle) {
    worldTransform_.rotation_.y += angle;
    worldTransform_.UpdateMatrix();
}