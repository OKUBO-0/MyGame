#pragma once
#include "FadeState.h"

class FadeStateNone : public FadeState {
public:
    void Update(Fade* fade) override;
};