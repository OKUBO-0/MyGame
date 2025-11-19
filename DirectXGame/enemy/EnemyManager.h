#pragma once

#include <vector>
#include <string>
#include <KamataEngine.h>
#include "Enemy.h"
#include "Player.h"

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
    /// コンストラクタ
    /// 敵管理用の初期値を設定する
    /// </summary>
    EnemyManager();

    /// <summary>
    /// デストラクタ
    /// 使用したリソースを解放する
    /// </summary>
    ~EnemyManager();

    /// <summary>
    /// 初期化処理
    /// CSVファイルから敵を生成し、プレイヤー参照を設定する
    /// </summary>
    /// <param name="csvPath">敵配置情報が記載されたCSVファイルのパス</param>
    /// <param name="player">関連付けるプレイヤー</param>
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
    /// <param name="camera">描画に使用するカメラ</param>
    void Draw(KamataEngine::Camera* camera);

    /// <summary>
    /// 敵リストを取得する
    /// </summary>
    /// <returns>敵ポインタのベクター参照</returns>
    const std::vector<Enemy*>& GetEnemies() const { return enemies_; }

    /// <summary>
    /// CSVファイルから敵を生成する
    /// </summary>
    /// <param name="filePath">敵配置情報が記載されたCSVファイルのパス</param>
    void SpawnEnemiesFromCSV(const std::string& filePath);

private:
    std::vector<Enemy*> enemies_; ///< 敵リスト
    Player* player_ = nullptr;    ///< プレイヤー参照
};