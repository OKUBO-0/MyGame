#pragma once
class Fade;

class FadeState {
public:
    virtual ~FadeState() = default;

    virtual void Enter(Fade* fade) {}
    virtual void Update(Fade* fade) = 0;
    virtual void Exit(Fade* fade) {}
};