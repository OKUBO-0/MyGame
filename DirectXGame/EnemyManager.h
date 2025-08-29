#pragma once
#include "Enemy.h"
#include "Player.h"
#include <vector>
#include <string>

class EnemyManager {
public:
    EnemyManager();
    ~EnemyManager();

    // 初期化（CSVファイルから敵配置を読み込む）
    void Initialize(const std::string& csvPath, Player* player);

    // 更新
    void Update();

    // 描画
    void Draw();

private:
    std::vector<Enemy*> enemies_;  // 複数の敵を管理

    // CSVから座標を読み込む
    std::vector<KamataEngine::Vector3> LoadEnemyPositionsFromCSV(const std::string& filePath);

    // プレイヤー参照
    Player* player_ = nullptr;
};