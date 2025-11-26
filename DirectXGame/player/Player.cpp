#include "Player.h"
using namespace KamataEngine;

Player::Player() {}
Player::~Player() {
    // プレイヤーモデルを解放
    delete playerModel_;

    // 弾インスタンスを解放
    for (auto b : bullets_) delete b;
    bullets_.clear();

    // パーティクルインスタンスを解放
    for (auto p : effects_) delete p;
    effects_.clear();
}

void Player::Initialize() {
    // 入力とカメラを初期化
    input_ = Input::GetInstance();
    camera_.Initialize();

    // プレイヤーモデルを読み込み
    playerModel_ = Model::CreateFromOBJ("octopus");

    // ワールドトランスフォーム初期化（位置を原点に設定）
    worldTransform_.Initialize();
    worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };

    // 初期ステータス設定
    level_ = 1;
    nextLevelExp_ = 50;
    bulletPower_ = 1;
    bulletCooldown_ = 1.0f;
    maxLifeStock_ = 3;

    exp_ = 0;
    totalExp_ = 0;
}

void Player::Update() {
    // プレイヤー移動入力処理
    const float moveSpeed = 0.2f;
    Vector3 move = { 0.0f, 0.0f, 0.0f };

    if (input_->PushKey(DIK_W)) move.z += moveSpeed;
    if (input_->PushKey(DIK_S)) move.z -= moveSpeed;
    if (input_->PushKey(DIK_A)) move.x -= moveSpeed;
    if (input_->PushKey(DIK_D)) move.x += moveSpeed;

    // 斜め移動補正：移動ベクトルを正規化して速度一定化
    float moveLen = std::sqrt(move.x * move.x + move.z * move.z);
    if (moveLen > 0.0f) {
        move.x = (move.x / moveLen) * moveSpeed;
        move.z = (move.z / moveLen) * moveSpeed;

        // 座標更新
        worldTransform_.translation_.x += move.x;
        worldTransform_.translation_.z += move.z;

        // プレイヤーの向きを移動方向に合わせる
        worldTransform_.rotation_.y = std::atan2(move.x, move.z);

        // パーティクル生成間隔制御（一定時間ごとに生成）
        effectTimer_ += 0.016f;
        if (effectTimer_ >= effectInterval_) {
            RippleEffect* e = new RippleEffect();
            e->Initialize(worldTransform_.translation_);
            effects_.push_back(e);
            effectTimer_ = 0.0f;
        }
    }

    bulletTimer_ += 0.016f;

    // 無敵時間処理（点滅演出）
    if (invincible_) {
        invincibleTimer_ -= 0.016f;
        if (invincibleTimer_ <= 0.0f) {
            invincible_ = false;
            visible_ = true;
        }
        else {
            int blinkFrame = static_cast<int>(invincibleTimer_ * 10.0f);
            visible_ = (blinkFrame % 2 == 0);
        }
    }

    // 射程内の敵探索（最も近い敵を狙う）
    Vector3 nearestDir = { 0.0f, 0.0f, 1.0f };
    float minDistSq = FLT_MAX;
    bool enemyInRange = false;

    if (enemyManager_) {
        for (auto enemy : enemyManager_->GetEnemies()) {
            if (!enemy->IsActive()) continue;

            Vector3 ePos = enemy->GetPosition();
            Vector3 pPos = worldTransform_.translation_;
            float dx = ePos.x - pPos.x;
            float dz = ePos.z - pPos.z;
            float distSq = dx * dx + dz * dz;

            if (distSq <= range_ * range_) {
                enemyInRange = true;
                if (distSq < minDistSq) {
                    minDistSq = distSq;
                    nearestDir = { dx, 0.0f, dz };
                }
            }
        }
    }

    // 敵が射程内にいる場合は向きを敵方向へ
    if (enemyInRange) {
        float len = std::sqrt(nearestDir.x * nearestDir.x + nearestDir.z * nearestDir.z);
        if (len > 0.0f) {
            nearestDir.x /= len;
            nearestDir.z /= len;
            worldTransform_.rotation_.y = std::atan2(nearestDir.x, nearestDir.z);
        }
    }
    // 敵がいない場合は移動方向に向きを合わせる
    else if (move.x != 0.0f || move.z != 0.0f) {
        worldTransform_.rotation_.y = std::atan2(move.x, move.z);
    }

    // 弾発射処理（クールダウン経過後、敵が射程内なら発射）
    if (bulletTimer_ >= bulletCooldown_ && enemyInRange) {
        float len = std::sqrt(nearestDir.x * nearestDir.x + nearestDir.z * nearestDir.z);
        if (len > 0.0f) {
            nearestDir.x /= len;
            nearestDir.z /= len;
        }

        Bullet* bullet = new Bullet();
        bullet->Initialize(worldTransform_.translation_, nearestDir, 0.5f); // 弾速は固定値
        bullet->SetDamage(bulletPower_); // 攻撃力を設定
        bullets_.push_back(bullet);
        bulletTimer_ = 0.0f;
    }

    // 弾更新と破棄（非アクティブになった弾を削除）
    for (auto it = bullets_.begin(); it != bullets_.end(); ) {
        Bullet* bullet = *it;
        bullet->Update(worldTransform_.translation_);
        if (!bullet->IsActive()) {
            delete bullet;
            it = bullets_.erase(it);
        }
        else {
            ++it;
        }
    }

    // パーティクル更新と破棄（寿命が尽きたものを削除）
    for (auto it = effects_.begin(); it != effects_.end();) {
        RippleEffect* e = *it;
        e->Update();
        if (!e->IsActive()) {
            delete e;
            it = effects_.erase(it);
        }
        else {
            ++it;
        }
    }

    // カメラ位置をプレイヤーに追従させる
    camera_.translation_.x = worldTransform_.translation_.x;
    camera_.translation_.z = worldTransform_.translation_.z;
    camera_.UpdateMatrix();

    // ワールド行列を更新
    worldTransform_.UpdateMatrix();
}

void Player::Draw() {
    // プレイヤー本体描画（無敵時の点滅に応じて表示制御）
    if (visible_) {
        playerModel_->Draw(worldTransform_, camera_);
    }

    // 弾描画
    for (auto bullet : bullets_) {
        bullet->Draw(&camera_);
    }

    // パーティクル描画
    for (auto e : effects_) {
        e->Draw(&camera_);
    }
}

void Player::TakeDamage() {
    // 無敵状態ならダメージを受けない
    if (invincible_) return;

    // HP減少と無敵時間開始
    lifeStock_--;
    invincible_ = true;
    invincibleTimer_ = 1.0f; // 1秒間無敵
    visible_ = false;        // ダメージ直後は非表示にして点滅演出開始
}

void Player::AddEXP(int amount) {
    // 経験値加算とレベルアップ判定
    exp_ += amount;
    totalExp_ += amount;
    while (exp_ >= nextLevelExp_) {
        exp_ -= nextLevelExp_;
        level_++;
        nextLevelExp_ = static_cast<int>(nextLevelExp_ * 1.5f); // 次の必要経験値を増加
        levelUpRequested_ = true; // レベルアップ演出要求フラグを立てる
    }
}

void Player::UpgradeBulletPower() {
    // 弾の攻撃力を強化
    bulletPower_ += 1;
}

void Player::UpgradeBulletCooldown() {
    // 弾の発射間隔を短縮
    bulletCooldown_ -= 0.05f;
}

void Player::RecoverHP() {
    // HPを回復（最大値を超えないよう制御）
    lifeStock_ += 1;
    if (lifeStock_ > maxLifeStock_) {
        lifeStock_ = maxLifeStock_;
    }
}