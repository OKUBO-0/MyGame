#include <KamataEngine.h>

using namespace KamataEngine;
using namespace MathUtility;

void WorldTransform::UpdateMatrix() {
    // オブジェクトの拡大縮小を表すスケーリング行列を生成
    Matrix4x4 matScale = MakeScaleMatrix(scale_);

    // 各軸の回転行列を生成（X, Y, Z）
    Matrix4x4 matRotX = MakeRotateXMatrix(rotation_.x);
    Matrix4x4 matRotY = MakeRotateYMatrix(rotation_.y);
    Matrix4x4 matRotZ = MakeRotateZMatrix(rotation_.z);

    // 回転行列を合成（Z→X→Y の順で適用）
    // この順序により、モデルの回転が自然に見えるよう調整されている
    Matrix4x4 matRot = matRotZ * matRotX * matRotY;

    // オブジェクトの位置を表す平行移動行列を生成
    Matrix4x4 matTrans = MakeTranslateMatrix(translation_);

    // スケーリング → 回転 → 平行移動 の順で行列を合成
    // これにより、拡大縮小や回転を適用した後に最終的な位置へ移動する
    matWorld_ = matScale * matRot * matTrans;

    // 計算したワールド行列を定数バッファへ転送し、描画処理で利用可能にする
    this->TransferMatrix();
}