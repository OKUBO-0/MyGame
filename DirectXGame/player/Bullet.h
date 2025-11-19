#pragma once
#include <KamataEngine.h>

/// <summary>
/// 弾（Bullet）を表すクラス。
/// 初期化・更新・描画を行い、攻撃力や速度、方向などの状態を管理する。
/// </summary>
class Bullet {
public:
    /// <summary>
    /// コンストラクタ
    /// 弾の初期値を設定する
    /// </summary>
    Bullet();

    /// <summary>
    /// デストラクタ
    /// 使用したリソースを解放する
    /// </summary>
    ~Bullet();

    /// <summary>
    /// 弾を初期化する
    /// </summary>
    /// <param name="startPos">弾の開始位置</param>
    /// <param name="direction">弾の進行方向</param>
    /// <param name="speed">弾の速度（デフォルト0.5f）</param>
    void Initialize(const KamataEngine::Vector3& startPos, const KamataEngine::Vector3& direction, float speed = 0.5f);

    /// <summary>
    /// 毎フレーム更新処理
    /// 弾の移動やプレイヤーとの関連処理を行う
    /// </summary>
    /// <param name="playerPos">プレイヤーの座標</param>
    void Update(const KamataEngine::Vector3& playerPos);

    /// <summary>
    /// 描画処理
    /// 弾モデルをカメラに基づいて描画する
    /// </summary>
    /// <param name="camera">描画に使用するカメラ</param>
    void Draw(KamataEngine::Camera* camera);

    /// <summary>
    /// 弾がアクティブかどうかを判定する
    /// </summary>
    /// <returns>true: アクティブ / false: 非アクティブ</returns>
    bool IsActive() const { return active_; }

    /// <summary>
    /// 弾を非アクティブ状態にする
    /// </summary>
    void Deactivate() { active_ = false; }

    /// <summary>
    /// 弾の現在位置を取得する
    /// </summary>
    /// <returns>弾の座標</returns>
    KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

    /// <summary>
    /// 弾の攻撃力を設定する
    /// </summary>
    /// <param name="value">設定する攻撃力</param>
    void SetDamage(int value) { damage_ = value; }

    /// <summary>
    /// 弾の攻撃力を取得する
    /// </summary>
    /// <returns>攻撃力の値</returns>
    int GetDamage() const { return damage_; }

private:
    KamataEngine::WorldTransform worldTransform_; ///< 弾のワールドトランスフォーム
    KamataEngine::Model* model_ = nullptr;        ///< 弾モデル

    KamataEngine::Vector3 direction_; ///< 弾の進行方向
    float speed_ = 0.5f;              ///< 弾の速度

    bool active_ = false;             ///< アクティブ状態フラグ
    int damage_ = 1;                  ///< 弾の攻撃力
};