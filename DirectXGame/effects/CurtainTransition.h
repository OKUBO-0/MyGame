#pragma once
#include <KamataEngine.h>
#include <memory>

class CurtainTransition {
public:
    enum class State {
        kNone,
        kClose,
        kOpen,
        kFinished
    };

    void Initialize();
    void StartClose(float speed = 20.0f);
    void StartOpen(float speed = 20.0f);
    void Update();
    void Draw();

    bool IsFinished() const { return state_ == State::kFinished; }
    State GetState() const { return state_; }

private:
    std::unique_ptr<KamataEngine::Sprite> topCurtain_;
    std::unique_ptr<KamataEngine::Sprite> bottomCurtain_;

    float speed_ = 20.0f;
    State state_ = State::kNone;
};