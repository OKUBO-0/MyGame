#pragma once
#include <KamataEngine.h>
#include <memory>

class FadeState;

class Fade {
public:
    void Initialize();
    void Update();
    void Draw();

    void StartFadeIn(float speed = kDefaultFadeSpeed);
    void StartFadeOut(float speed = kDefaultFadeSpeed);

    bool IsFinished() const { return finished_; }

    void ChangeState(FadeState* newState);

public:
    std::unique_ptr<KamataEngine::Sprite> fadeSprite_;
    float alpha_ = 1.0f;
    float speed_ = 0.02f;
    bool finished_ = false;

private:
    std::unique_ptr<FadeState> state_;

    static constexpr int kScreenWidth = 1280;
    static constexpr int kScreenHeight = 720;
    static constexpr const char* kBlackTexturePath = "color/black.png";

public:
    static const float kDefaultFadeSpeed;
};