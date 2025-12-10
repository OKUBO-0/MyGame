#pragma once
#include <cstdint>

/// <summary>
/// ゲーム全体で共有されるデータを管理するクラス。
/// プレイヤーが獲得した総経験値などを保持する。
/// </summary>
class GameData {
public:
    /// <summary>
    /// プレイヤーが獲得した総経験値
    /// </summary>
    static int32_t totalExp;
};