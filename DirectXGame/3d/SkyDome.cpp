#include "SkyDome.h"

using namespace KamataEngine;

SkyDome::SkyDome() {
}

SkyDome::~SkyDome() {
	delete skyModel_;
}

void SkyDome::Initialize() {
	// カメラの初期化
	camera_.Initialize();

	// プレイヤーモデルの読み込み
	skyModel_ = Model::CreateFromOBJ("skydome");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
}

void SkyDome::Update() {
}

void SkyDome::Draw() {
	// モデルの描画
	skyModel_->Draw(worldTransform_, camera_);
}