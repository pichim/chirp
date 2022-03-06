#ifndef CHIRP_H_
#define CHIRP_H_

#include <math.h>
#include <stdint.h>

#define M_PI 3.14159265358979323846  /* pi */

class CHIRP
{
public:

    CHIRP() {};
    CHIRP(float f0, float f1, int N, float Ts);

    float operator()() {
        return update();
    }

    ~CHIRP();

    void    setParameters(float f0, float f1, uint32_t N, float Ts);
    void    reset();
    float   update();
    float   update(float& sinarg, float& fchirp);
    
    float   read_fchirp();
    float   read_sinarg();
    float   read_exc();
    
private:

    struct CHIRP_DATA {
        CHIRP_DATA() : f0(0.0f), f1(0.0f), t1(0.0f), Ts(0.0f), beta(0.0f), k0(0.0f), k1(0.0f), ii(0), N(0) {}
        float   f0, f1, t1, Ts, beta, k0, k1;
        int     ii, N;
    };

    CHIRP_DATA* chirp_data;

    float   f0_, f1_, t1_, Ts_, beta_, k0_, k1_;
    int     ii_, N_;
    float   exc_, fchirp_, sinarg_;
    
    void    calc_beta();

};

#endif