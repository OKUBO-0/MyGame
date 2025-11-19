#include "GridPlane.h"
using namespace KamataEngine;

GridPlane::GridPlane() {}
GridPlane::~GridPlane() {
    delete planeModel_;
}

void GridPlane::Initialize() {
    // ワールドトランスフォームの初期化
    worldTransform_.Initialize();

    // 平面モデルを読み込む（Plane.obj を用意しておく）
    planeModel_ = Model::CreateFromOBJ("Plane");

    // 床として広げる
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
    worldTransform_.scale_ = { 1000.0f, 1.0f, 1000.0f };
    worldTransform_.rotation_ = { 0.0f, 0.0f, 0.0f };

    // UVタイルの設定（テクスチャを繰り返してグリッドを均等に表示する）
    // tileSize = 1.0f のテクスチャピクセル/ワールド単位換算に対するタイル数を計算する
    // 必要に応じて tileSize を変更して1タイルの大きさを調整してください
    const float tileSize = 5.0f;
    const float uvScaleX = worldTransform_.scale_.x / tileSize;
    const float uvScaleY = worldTransform_.scale_.z / tileSize;

    if (planeModel_) {
        // モデル内の全メッシュのマテリアルにUVスケールを設定
        for (auto& mesh : planeModel_->GetMeshes()) {
            if (!mesh) continue;
            Material* material = mesh->GetMaterial();
            if (!material) continue;

            // X,Z方向に対応するようにUVスケールを設定（Yは未使用）
            material->uvScale_.x = uvScaleX;
            material->uvScale_.y = uvScaleY;
            // 変更を定数バッファへ反映
            material->Update();
        }
    }
}

void GridPlane::Update() {
    worldTransform_.UpdateMatrix();
}

void GridPlane::Draw(Camera* camera) {
    if (planeModel_) {
        planeModel_->Draw(worldTransform_, *camera);
    }
}