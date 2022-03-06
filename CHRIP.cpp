/*
   CHIRP: chirp sequence generator

    instantiate option: CHIRP(float f0, float f1, int N, float Ts)

    notes: INSERT DESCRIPTION HERE

        autor: M.E. Peter
*/

#include "CHIRP.h"

using namespace std;

CHIRP::CHIRP(float f0, float f1, int N, float Ts)
{
    setParameters(f0, f1, N, Ts);
}

CHIRP::~CHIRP() {}

void CHIRP::setParameters(float f0, float f1, uint32_t N, float Ts)
{

    chirp_data = new CHIRP_DATA;

    chirp_data->f0 = f0;
    chirp_data->f1 = f1;
    chirp_data->t1 = static_cast<float>( (N - 1) ) * Ts;
    chirp_data->Ts = Ts;
    // chirp_data->beta = f0;
    // chirp_data->k0 = f0;
    // chirp_data->k1 = f0;
    chirp_data->ii = 0;
    chirp_data->N  = N;

    /* assign member variables */
    f0_ = f0;
    f1_ = f1;
    t1_ = static_cast<float>( (N - 1) ) * Ts;
    Ts_ = Ts;
    ii_ = 0;
    N_  = N;
    
    /* adjust f1 so that the sweep stops at angle 0 or pi */
    calc_beta();
    float sinarg_end = (f0_*(powf(beta_, t1_) - 1.0f)/logf(beta_));
    float kr = floorf(sinarg_end);
    /* solve correction through iterative inserting */
    for(uint8_t i = 0; i < 13; i++) {
        f1_ = f0_ + kr*logf(powf(f1_/f0_, 1.0f/t1_));
    }
    
    calc_beta();
    k0_ = 2.0f*M_PI/logf(beta_);
    k1_ = k0_*f0_;
    
    reset();
}

void CHIRP::reset()
{
    fchirp_ = 0.0f;
    sinarg_ = 0.0f;
    exc_ = 0.0f;
    ii_ = 0;
    
    /* debug */
    // printf("%9.6e; %9.6e; %9.6e; %9.6e; %9i; %9i; %9.6e; %9.6e; %9.6e;\r\n", f0_, f1_, t1_, Ts_, ii_, N_, beta_, k0_, k1_);
}

float CHIRP::update()
{
    fchirp_ = f0_*powf(beta_, (float)ii_*Ts_);
    sinarg_ = k0_*fchirp_ - k1_;
    exc_ = sinf(sinarg_);
    ii_++;
    // if(ii_++ == N_ - 1) reset();
    return exc_;
}

float CHIRP::update(float& sinarg, float& fchirp)
{
    update();
    sinarg = sinarg_;
    fchirp = fchirp_;
    return exc_;
}

float CHIRP::read_fchirp()
{
    return fchirp_;
}

float CHIRP::read_sinarg()
{
    return sinarg_;
}

float CHIRP::read_exc()
{
    return exc_;
}

void CHIRP::calc_beta()
{
    beta_ = powf(f1_/f0_, 1.0f/t1_);
}

