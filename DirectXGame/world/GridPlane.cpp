#include "GridPlane.h"
using namespace KamataEngine;

void GridPlane::Initialize() {
    // ワールド座標系の初期化
    worldTransform_.Initialize();

    // 床モデルを読み込み（グリッド模様付き）
    planeModel_.reset(Model::CreateFromOBJ("Plane"));

    // 床の位置・スケール・回転を設定
    worldTransform_.translation_ = { 0.0f, -2.0f, 0.0f };   // Yを下げて地面に配置
    worldTransform_.scale_ = { kGroundScale, 1.0f, kGroundScale }; // X,Z方向に広げる
    worldTransform_.rotation_ = { 0.0f, 0.0f, 0.0f };       // 回転は不要なのでゼロ

    // --- UVスケール算出 ---
    // 意図: 床の広さに応じてテクスチャの繰り返し数を決定し、模様を均等に表示する
    const float uvScaleX = worldTransform_.scale_.x / kTileSize;
    const float uvScaleY = worldTransform_.scale_.z / kTileSize;

    if (planeModel_) {
        for (auto& mesh : planeModel_->GetMeshes()) {
            if (!mesh) continue;
            Material* material = mesh->GetMaterial();
            if (!material) continue;

            // グリッド模様を均等に表示するために UVスケールを反映
            material->uvScale_.x = uvScaleX;
            material->uvScale_.y = uvScaleY;
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