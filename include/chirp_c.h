#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct chirp_s
{
    float f0, f1, t1, Ts, beta, k0, k1;
    uint32_t count, N;
    float exc, fchirp, sinarg;
} chirp_t;

void chirpInit(chirp_t *chirp, float f0, float f1, uint32_t N, float Ts);
void chirpResetCount(chirp_t *chirp);
void chirpResetSignals(chirp_t *chirp);
bool chirpUpdate(chirp_t *chirp);