#include "FadeStateOut.h"
#include "Fade.h"
#include "FadeStateNone.h"

void FadeStateOut::Update(Fade* fade) {
    fade->alpha_ += fade->speed_;

    if (fade->alpha_ >= 1.0f) {
        fade->alpha_ = 1.0f;
        fade->finished_ = true;
        fade->ChangeState(new FadeStateNone());
    }
}