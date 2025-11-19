#pragma once
#include <KamataEngine.h>

/// グリッド床を表示するクラス
class GridPlane {
public:
    GridPlane();
    ~GridPlane();

    /// 初期化処理
    void Initialize();

    /// 毎フレーム更新処理
    void Update();

    /// 描画処理
    void Draw(KamataEngine::Camera* camera);

private:
    KamataEngine::WorldTransform worldTransform_; // ワールド変換
    KamataEngine::Model* planeModel_ = nullptr;       // 平面モデル
};