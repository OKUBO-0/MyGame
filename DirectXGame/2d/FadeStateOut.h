#pragma once
#include "FadeState.h"

class FadeStateOut : public FadeState {
public:
    void Update(Fade* fade) override;
};