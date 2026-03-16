#include "FadeStateIn.h"
#include "Fade.h"
#include "FadeStateStay.h"

void FadeStateIn::Update(Fade* fade) {
    fade->alpha_ -= fade->speed_;

    if (fade->alpha_ <= 0.0f) {
        fade->alpha_ = 0.0f;
        fade->finished_ = true;
        fade->ChangeState(new FadeStateStay());
    }
}