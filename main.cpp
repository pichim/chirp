#include <iostream>
#include <pthread.h>
#include <fstream>

#include "CHIRP.h"

typedef struct chirp_s
{
    float f0, f1, t1, Ts, beta, k0, k1;
    uint32_t ii, N;
    float exc, fchirp, sinarg;
} chirp_t;

void chirpInit(chirp_t *chirp, float f0, float f1, uint32_t N, float Ts)
{
    chirp->f0 = f0;
    chirp->f1 = f1;
    chirp->t1 = static_cast<float>((N - 1)) * Ts;
    chirp->Ts = Ts;
    chirp->ii = 0;
    chirp->N = N;
}

void chirpOptimizeParametersForEndFrequency(chirp_t *chirp)
{
    // adjust f1 so that the sweep stops at angle 0 or pi
    chirp->beta = powf(chirp->f1 / chirp->f0, 1.0f / chirp->t1);
    float sinargEnd = (chirp->f0 * (powf(chirp->beta, chirp->t1) - 1.0f) / logf(chirp->beta));
    float kr = floorf(sinargEnd);

    // solve correction through iterative inserting
    for (uint8_t i = 0; i < 13; i++)
    {
        chirp->f1 = chirp->f0 + kr * logf(powf(chirp->f1 / chirp->f0, 1.0f / chirp->t1));
    }

    // get final parameters
    chirp->beta = powf(chirp->f1 / chirp->f0, 1.0f / chirp->t1);
    chirp->k0 = 2.0f * M_PI / logf(chirp->beta);
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
        chirp->fchirp = chirp->f0 * powf(chirp->beta, static_cast<float>(chirp->ii) * chirp->Ts);
        chirp->sinarg = chirp->k0 * chirp->fchirp - chirp->k1;
        chirp->exc = sinf(chirp->sinarg);
        chirp->ii++;
        return true;
    }
}

int main(int argc, char *argv[])
{
    // set up chirp generator
    float Ts = 0.01f;
    float f0 = 0.2f;
    float f1 = 0.99f / 2.0f / Ts;
    uint32_t N = 1000;
    CHIRP *chirp = new CHIRP(f0, f1, N, Ts);

    chirp_t *chirp2 = new chirp_t;
    chirpInit(chirp2, f0, f1, N, Ts);
    chirpOptimizeParametersForEndFrequency(chirp2);
    chirpReset(chirp2);

    uint32_t cntr = 0;

    // open file for writing
    std::ofstream datafile("chirp_meas_01.txt");

    bool main_execute = true;
    while (main_execute)
    {
        cntr++;
        if (chirp->update())
        {
            std::cout << cntr << ", " << chirp->exc() << ", " << chirp->sinarg() << ", " << chirp->fchirp() << std::endl;
            datafile << cntr << ", " << chirp->exc() << ", " << chirp->sinarg() << ", " << chirp->fchirp() << std::endl;
        }
        /*if (chirpUpdate(chirp2))
        {
            std::cout << cntr << ", " << chirp2->exc << ", " << chirp2->sinarg << ", " << chirp2->fchirp << std::endl;
            datafile << cntr << ", " << chirp2->exc << ", " << chirp2->sinarg << ", " << chirp2->fchirp << std::endl;
        }*/
        else
        {
            std::cout << "---   chirp finished" << std::endl;
            main_execute = false;
        }
    }

    datafile.close();

    return 0;
}
