#pragma once

#include <vector>
#include <list>
#include <string>
#include <memory>
#include <KamataEngine.h>
#include "Enemy.h"
#include "Player.h"
#include "DeathParticle.h"
#include "ExpOrb.h"

class Player;
class Enemy;

/// <summary>
/// 敵キャラクターを一括管理するクラス。
/// CSVファイルから敵を生成し、更新・描画を行う。
/// プレイヤーとの関連付けも保持する。
/// </summary>
class EnemyManager {
public:
    /// <summary>
    /// 初期化処理
    /// CSVファイルから敵を生成し、プレイヤー参照を設定する
    /// </summary>
    void Initialize(const std::string& csvPath, Player* player);

    /// <summary>
    /// 毎フレーム更新処理
    /// 敵の挙動や状態を更新する
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// 敵モデルをカメラに基づいて描画する
    /// </summary>
    void Draw(KamataEngine::Camera* camera);

    /// <summary>
    /// 敵リストを取得する
    /// </summary>
    const std::vector<std::unique_ptr<Enemy>>& GetEnemies() const { return enemies_; }

    /// <summary>
    /// CSVファイルから敵を生成する
    /// </summary>
    void SpawnEnemiesFromCSV(const std::string& filePath);

    float RandomFloat(float min, float max);

private:
    std::vector<std::unique_ptr<Enemy>> enemies_; ///< 敵リスト（スマートポインタで管理）
    Player* player_ = nullptr;                    ///< プレイヤー参照

    std::list<std::unique_ptr<DeathParticle>> deathParticles_; ///< 死亡パーティクルリスト
    std::list<std::unique_ptr<ExpOrb>> expOrbs_;               ///< 経験値オーブリスト
};