#pragma once
#include "FadeState.h"

class FadeStateStay : public FadeState {
public:
    void Update(Fade* fade) override;
};