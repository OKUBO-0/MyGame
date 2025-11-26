#include "GridPlane.h"
using namespace KamataEngine;

GridPlane::GridPlane() {}
GridPlane::~GridPlane() {
    // 動的に生成した平面モデルを解放
    delete planeModel_;
}

void GridPlane::Initialize() {
    // ワールド座標系の初期化（位置・回転・スケールをリセット）
    worldTransform_.Initialize();

    // 床として利用する平面モデルを読み込む
    // Plane.obj は事前に用意されたグリッド用のモデル
    planeModel_ = Model::CreateFromOBJ("Plane");

    // 床の位置を少し下げ、広大なスケールで拡張する
    worldTransform_.translation_ = { 0.0f, -2.0f, 0.0f };   // Yを下げて地面に配置
    worldTransform_.scale_ = { 1000.0f, 1.0f, 1000.0f };    // X,Z方向に大きく広げる
    worldTransform_.rotation_ = { 0.0f, 0.0f, 0.0f };       // 回転は不要なのでゼロ

    // テクスチャの繰り返し設定（UVスケール）
    // tileSize は「1タイルの大きさ」を決める値で、床のスケールに応じて繰り返し数を算出する
    const float tileSize = 5.0f;
    const float uvScaleX = worldTransform_.scale_.x / tileSize; // X方向の繰り返し数
    const float uvScaleY = worldTransform_.scale_.z / tileSize; // Z方向の繰り返し数

    if (planeModel_) {
        // モデル内の各メッシュに対して UVスケールを設定
        for (auto& mesh : planeModel_->GetMeshes()) {
            if (!mesh) continue;
            Material* material = mesh->GetMaterial();
            if (!material) continue;

            // グリッド模様を均等に表示するために X,Z方向のUVスケールを反映
            material->uvScale_.x = uvScaleX;
            material->uvScale_.y = uvScaleY;

            // マテリアルの変更をGPU側に反映
            material->Update();
        }
    }
}

void GridPlane::Update() {
    // ワールド行列を更新（位置・回転・スケールを反映）
    worldTransform_.UpdateMatrix();
}

void GridPlane::Draw(Camera* camera) {
    // 平面モデルが存在する場合のみ描画
    if (planeModel_) {
        planeModel_->Draw(worldTransform_, *camera);
    }
}