#pragma once

#include <KamataEngine.h>
#include <cstdint>
#include <memory>

class GameStartController {
public:
    void Initialize();
    bool Update(KamataEngine::Input* input, KamataEngine::Audio* audio, uint32_t startSEHandle);
    void Draw() const;
    void Reset();

    bool IsWaiting() const { return waiting_; }

private:
    bool waiting_ = true;
    std::unique_ptr<KamataEngine::Sprite> overlaySprite_;
};
