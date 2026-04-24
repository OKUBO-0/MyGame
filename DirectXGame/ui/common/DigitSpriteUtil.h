#pragma once

#include <KamataEngine.h>
#include <cstdint>
#include <memory>
#include <vector>

namespace DirectXGame::DigitSpriteUtil {

KamataEngine::Vector2 CalculateDigitPosition(
    const KamataEngine::Vector2& basePosition,
    const KamataEngine::Vector2& size,
    float scale,
    int32_t index);

void UpdateDigitLayout(
    KamataEngine::Sprite& sprite,
    const KamataEngine::Vector2& basePosition,
    const KamataEngine::Vector2& size,
    float scale,
    int32_t index);

void SetDigitSprite(
    KamataEngine::Sprite& sprite,
    float digitWidth,
    const KamataEngine::Vector2& size,
    int32_t number);

template <typename SpriteContainer>
void SetNumberSprites(
    const SpriteContainer& sprites,
    float digitWidth,
    const KamataEngine::Vector2& size,
    int32_t number,
    int32_t initialDigit) {
    int32_t digit = initialDigit;
    for (size_t i = 0; i < sprites.size(); ++i) {
        if (!sprites[i]) {
            digit /= 10;
            continue;
        }

        const int32_t nowNumber = digit > 0 ? number / digit : 0;
        SetDigitSprite(*sprites[i], digitWidth, size, nowNumber);
        if (digit > 0) {
            number %= digit;
            digit /= 10;
        }
    }
}

} // namespace DirectXGame::DigitSpriteUtil
