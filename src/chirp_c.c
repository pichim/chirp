#include "..\include/chirp_c.h"
#include "..\include/maths.h"

#include <math.h>

void chirpInit(chirp_t *chirp, float f0, float f1, uint32_t N, float Ts)
{
    chirp->f0 = f0;
    chirp->f1 = f1;
    chirp->t1 = (float)(N - 1) * Ts;
    chirp->Ts = Ts;
    chirp->ii = 0;
    chirp->N = N;
    chirpOptimizeParametersForEndFrequency(chirp);
    chirpReset(chirp);
}

void chirpOptimizeParametersForEndFrequency(chirp_t *chirp)
{
    // adjust f1 so that the sweep stops at angle 0 or pi
    chirp->beta = pow_approx(chirp->f1 / chirp->f0, 1.0f / chirp->t1);
    float sinargEnd = (chirp->f0 * (pow_approx(chirp->beta, chirp->t1) - 1.0f) / log_approx(chirp->beta));
    float kr = floorf(sinargEnd);

    // solve correction through iterative inserting
    for (uint8_t i = 0; i < 13; i++)
    {
        chirp->f1 = chirp->f0 + kr * log_approx(pow_approx(chirp->f1 / chirp->f0, 1.0f / chirp->t1));
    }

    // get final parameters
    chirp->beta = pow_approx(chirp->f1 / chirp->f0, 1.0f / chirp->t1);
    chirp->k0 = 2.0f * M_PI / log_approx(chirp->beta);
    chirp->k1 = chirp->k0 * chirp->f0;
}

void chirpReset(chirp_t *chirp)
{
    chirp->fchirp = 0.0f;
    chirp->sinarg = 0.0f;
    chirp->exc = 0.0f;
    chirp->ii = 0;
}

bool chirpUpdate(chirp_t *chirp)
{
    if (chirp->ii == chirp->N)
    {
        return false;
    }
    else
    {
        chirp->fchirp = chirp->f0 * pow_approx(chirp->beta, (float)(chirp->ii) * chirp->Ts);
        chirp->sinarg = chirp->k0 * chirp->fchirp - chirp->k1;
        chirp->exc = sinf(chirp->sinarg);
        chirp->ii++;
        return true;
    }
}