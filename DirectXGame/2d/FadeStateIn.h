#pragma once
#include "FadeState.h"

class FadeStateIn : public FadeState {
public:
    void Update(Fade* fade) override;
};