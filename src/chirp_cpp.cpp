#include "..\include/chirp_cpp.h"

#include <math.h>

CHIRP::CHIRP(float _f0, float _f1, uint32_t _N, float _Ts)
{
    setParameters(_f0, _f1, _N, _Ts);
}

CHIRP::~CHIRP() {}

void CHIRP::setParameters(float _f0, float _f1, uint32_t _N, float _Ts)
{
    m_f0 = _f0;
    m_f1 = _f1;
    m_t1 = (float)(_N - 1) * _Ts;
    m_Ts = _Ts;
    resetCount();
    m_N = _N;
    // optimizeParametersForEndFrequency(m_f0, m_f1, m_beta, m_t1, m_k0, m_k1);
    m_beta = powf(_f1 / _f0, 1.0f / m_t1);
    m_k0 = 2.0f * M_PI / logf(m_beta);
    m_k1 = m_k0 * _f0;
    resetSignals();
}

void CHIRP::resetCount()
{
    m_count = 0;
}

void CHIRP::optimizeParametersForEndFrequency(const float &_f0, float &_f1, float &_beta, const float &_t1, float &_k0, float &_k1)
{
    // adjust f1 so that the sweep stops at angle 0 or pi
    _beta = powf(_f1 / _f0, 1.0f / _t1);
    float sinargEnd = (_f0 * (powf(_beta, _t1) - 1.0f) / logf(_beta));
    float kr = floorf(sinargEnd);

    // solve correction through iterative inserting
    for (uint8_t i = 0; i < 13; i++)
    {
        _f1 = _f0 + kr * logf(powf(_f1 / _f0, 1.0f / _t1));
    }

    // get final parameters
    _beta = powf(_f1 / _f0, 1.0f / _t1);
    _k0 = 2.0f * M_PI / logf(_beta);
    _k1 = _k0 * _f0;
}

void CHIRP::resetSignals()
{
    m_fchirp = 0.0f;
    m_sinarg = 0.0f;
    m_exc = 0.0f;
}

bool CHIRP::update()
{
    if (m_count == m_N)
    {
        resetSignals();
        return false;
    }
    else
    {
        m_fchirp = m_f0 * powf(m_beta, (float)(m_count) * m_Ts);
        m_sinarg = m_k0 * m_fchirp - m_k1;
        m_exc = sinf(m_sinarg);
        m_count++;
        return true;
    }
}

float CHIRP::fchirp()
{
    return m_fchirp;
}

float CHIRP::sinarg()
{
    return m_sinarg;
}

float CHIRP::exc()
{
    return m_exc;
}

/*
float CHIRP::calc_beta(float _f0, float _f1, float _t1)
{
    return powf(_f1/_f0, 1.0f/_t1);
}
*/
