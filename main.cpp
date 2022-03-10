#include <iostream>
#include <fstream>

#include "chirp_cpp.h"
#include "chirp_c.h"
#include "filter.h"

int main(int argc, char *argv[])
{
    bool do_plot_in_terminal = false;
    bool do_use_cpp_implementation = false;
    bool do_use_filter = true;

    // set up chirp generator
    float Ts = 1.0e-4f;
    float f0 = 0.2f;
    float f1 = 1.0e3f;
    uint32_t N = 50e3;
    CHIRP *chirp_cpp = new CHIRP();
    chirp_cpp->init(f0, f1, N, Ts);

    chirp_t *chirp_c = new chirp_t;
    chirpInit(chirp_c, f0, f1, N, Ts);

    // biquadFilter_t *filter_c = new biquadFilter_t;
    // biquadFilterInitLPF(filter_c, 200.0f, (uint32_t)(Ts * 1.0e6f));
    pt1TustinFilter_t *filter_c = new pt1TustinFilter_t;
    pt1TustinFilterInit(filter_c, 200.0f, (uint32_t)(Ts * 1.0e6f));

    uint32_t cntr = 0;

    // open file for writing
    std::ofstream datafile("output/chirp_meas_01.txt");

    bool main_execute = true;
    while (main_execute)
    {
        cntr++;
        if (do_use_cpp_implementation)
        {
            if (chirp_cpp->update())
            {
                float exc;
                if (do_use_filter)
                {
                    // exc = biquadFilterApply(filter_c, chirp_cpp->exc());
                    exc = pt1TustinFilterApply(filter_c, chirp_cpp->exc());
                }
                else
                {
                    exc = chirp_cpp->exc();
                }
                if (do_plot_in_terminal)
                {
                    std::cout << cntr << ", " << exc << ", " << chirp_cpp->sinarg() << ", " << chirp_cpp->fchirp() << std::endl;
                }
                datafile << cntr << ", " << exc << ", " << chirp_cpp->sinarg() << ", " << chirp_cpp->fchirp() << std::endl;
            }
            else
            {
                std::cout << "---   chirp finished" << std::endl;
                main_execute = false;
            }
        }
        else
        {
            if (chirpUpdate(chirp_c))
            {
                float exc;
                if (do_use_filter)
                {
                    // exc = biquadFilterApply(filter_c, chirp_c->exc);
                    exc = pt1TustinFilterApply(filter_c, chirp_c->exc);
                }
                else
                {
                    exc = chirp_c->exc;
                }
                if (do_plot_in_terminal)
                {
                    std::cout << cntr << ", " << exc << ", " << chirp_c->sinarg << ", " << chirp_c->fchirp << std::endl;
                }
                datafile << cntr << ", " << exc << ", " << chirp_c->sinarg << ", " << chirp_c->fchirp << std::endl;
            }
            else
            {
                std::cout << "---   chirp finished" << std::endl;
                main_execute = false;
            }
        }
    }

    datafile.close();

    return 0;
}
