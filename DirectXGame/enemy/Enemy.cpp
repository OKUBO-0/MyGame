#include "Enemy.h"
using namespace KamataEngine;

Enemy::Enemy() {}

Enemy::~Enemy() {
    // 敵モデルを動的に生成しているため、破棄時に解放
    delete enemyModel_;
    enemyModel_ = nullptr;

    // オブジェクトカラーを解放
    delete objectColor_;
    objectColor_ = nullptr;
}

void Enemy::Initialize() {
    // ワールドトランスフォームを初期化（位置・回転・スケールをリセット）
    worldTransform_.Initialize();

    // 初期位置を原点に設定
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };

    // 敵をアクティブ状態にする
    active_ = true;

    // オブジェクトカラーを生成して初期化（デフォルト色）
    if (!objectColor_) {
        objectColor_ = new ObjectColor();
        objectColor_->Initialize();
        objectColor_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    }

    // 白テクスチャをロードしておく（既存UI等と共通の white1x1.png を使用）
    whiteTextureHandle_ = TextureManager::Load("color/white.png");
}

void Enemy::SetModelByType(int32_t type) {
    // 既存モデルがある場合は解放してから新しいモデルを設定
    if (enemyModel_) {
        delete enemyModel_;
        enemyModel_ = nullptr;
    }

    // 敵の種類に応じて異なるモデルを読み込む
    switch (type) {
    case 0:
        enemyModel_ = Model::CreateFromOBJ("Enemy1");
        break;
    case 1:
        enemyModel_ = Model::CreateFromOBJ("Enemy2");
        break;
    case 2:
        enemyModel_ = Model::CreateFromOBJ("Enemy3");
        break;
    case 3:
        enemyModel_ = Model::CreateFromOBJ("Enemy4");
        break;
    default:
        // デフォルトはタコモデル（octopus）
        enemyModel_ = Model::CreateFromOBJ("octopus");
        break;
    }

    // モデルを切り替えた場合もオブジェクトカラーを用意しておく
    if (!objectColor_) {
        objectColor_ = new ObjectColor();
        objectColor_->Initialize();
        objectColor_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    }
}

void Enemy::Update() {
    // 非アクティブ状態なら処理しない
    if (!active_) { return; }

    // ヒット点滅の時間経過処理（60FPS前提で固定dt）
    const float kDeltaTime = 0.016f;
    if (hitFlashTimer_ > 0.0f) {
        hitFlashTimer_ -= kDeltaTime;
        if (hitFlashTimer_ <= 0.0f) {
            hitFlashTimer_ = 0.0f;
            // 点滅終了 → 元の色に戻す
            if (objectColor_) { objectColor_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); }
        }
    }

    // ノックバック処理：ノックバック中は追尾を行わず押し出す
    if (knockbackTimer_ > 0.0f) {
        worldTransform_.translation_.x += knockbackVelocity_.x;
        worldTransform_.translation_.z += knockbackVelocity_.z;

        // 減衰
        knockbackVelocity_.x *= 0.88f;
        knockbackVelocity_.z *= 0.88f;

        knockbackTimer_ -= kDeltaTime;
        if (knockbackTimer_ <= 0.0f) {
            knockbackTimer_ = 0.0f;
            knockbackVelocity_ = { 0.0f, 0.0f, 0.0f };
        }
    }
    else {
        // 通常の追尾処理
        if (player_) {
            const Vector3& playerPos = player_->GetWorldPosition();
            Vector3 dir = {
                playerPos.x - worldTransform_.translation_.x,
                0.0f,
                playerPos.z - worldTransform_.translation_.z
            };

            float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
            if (len > 0.0f) {
                dir.x /= len;
                dir.z /= len;

                worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);

                worldTransform_.translation_.x += dir.x * speed_;
                worldTransform_.translation_.z += dir.z * speed_;
            }
        }
    }

    worldTransform_.UpdateMatrix();
}

void Enemy::Draw(KamataEngine::Camera* camera) {
    // 非アクティブまたはモデル未設定なら描画しない
    if (!active_ || !enemyModel_) { return; }

    // ヒット中は白テクスチャで上書きし、かつ objectColor_ を渡して強く乗算する
    if (hitFlashTimer_ > 0.0f && whiteTextureHandle_ != 0 && objectColor_) {
        // 白テクスチャ＋白カラーで完全な白に
        enemyModel_->Draw(worldTransform_, *camera, whiteTextureHandle_, objectColor_);
    }
    else {
        // 通常描画（ObjectColor を渡しておいても問題ない）
        if (objectColor_) {
            enemyModel_->Draw(worldTransform_, *camera, objectColor_);
        }
        else {
            enemyModel_->Draw(worldTransform_, *camera);
        }
    }
}

void Enemy::TakeDamage(int32_t damage, const Vector3& knockDir, float strength) {
    // ダメージを受けてHPを減少
    hp_ -= damage;

    // HPが0以下になったら非アクティブ化
    if (hp_ <= 0) {
        Deactivate();
        justDied_ = true; // 死亡直後フラグを立てる
        return;
    }

    // 白フラッシュ
    hitFlashTimer_ = kHitFlashDuration;
    if (objectColor_) {
        // 値を大きくしてより白く見せる
        objectColor_->SetColor({ 10.0f, 10.0f, 10.0f, 1.0f });
    }

    // ノックバックを適用（方向と強さが有効な場合）
    float len = std::sqrt(knockDir.x * knockDir.x + knockDir.z * knockDir.z);
    if (len > 0.0001f && strength > 0.0f) {
        Vector3 dir = knockDir;
        dir.x /= len;
        dir.z /= len;
        // strength を初速として使用
        knockbackVelocity_.x = dir.x * strength;
        knockbackVelocity_.z = dir.z * strength;
        knockbackTimer_ = kKnockbackDuration;
    }
}