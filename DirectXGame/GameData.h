// GameData.h
#pragma once

/// <summary>
/// ゲーム全体で共有するデータを保持するクラス（シングルトン的利用）
/// </summary>
class GameData {
public:
    static int totalEXP; ///< プレイヤーが獲得した総経験値
};