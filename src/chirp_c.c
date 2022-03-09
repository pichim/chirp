#include "..\include/chirp_c.h"
#include "..\include/maths.h"

#include <math.h>

void chirpInit(chirp_t *chirp, float f0, float f1, uint32_t N, float Ts)
{
    chirp->f0 = f0;
    chirp->f1 = f1;                  // not used
    chirp->t1 = (float)(N - 1) * Ts; // not used
    chirp->Ts = Ts;
    chirpResetCount(chirp);
    chirp->N = N;
    chirp->beta = pow_approx(chirp->f1 / chirp->f0, 1.0f / chirp->t1);
    chirp->k0 = 2.0f * M_PI / log_approx(chirp->beta);
    chirp->k1 = chirp->k0 * chirp->f0;
    chirpResetSignals(chirp);
}

void chirpResetCount(chirp_t *chirp)
{
    chirp->count = 0;
}

void chirpResetSignals(chirp_t *chirp)
{
    chirp->fchirp = 0.0f;
    chirp->sinarg = 0.0f;
    chirp->exc = 0.0f;
}

bool chirpUpdate(chirp_t *chirp)
{
    if (chirp->count == chirp->N)
    {
        chirpResetSignals(chirp);
        return false;
    }
    else
    {
        chirp->fchirp = chirp->f0 * pow_approx(chirp->beta, (float)(chirp->count) * chirp->Ts);
        chirp->sinarg = chirp->k0 * chirp->fchirp - chirp->k1;
        chirp->exc = sinf(chirp->sinarg);
        chirp->count++;
        return true;
    }
}