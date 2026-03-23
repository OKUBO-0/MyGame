#include "GameData.h"

namespace EngineLayer {

constexpr int32_t kInitialTotalExp = 0;
constexpr int32_t kInitialFinalLevel = 1;
constexpr int32_t kInitialTotalKillCount = 0;

} // namespace EngineLayer

int32_t GameData::totalExp = EngineLayer::kInitialTotalExp;
int32_t GameData::finalLevel = EngineLayer::kInitialFinalLevel;
int32_t GameData::totalKillCount = EngineLayer::kInitialTotalKillCount;
