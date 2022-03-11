#include <iostream>
#include <fstream>

#include "chirp_cpp.h"
#include "chirp_c.h"
#include "filter.h"

int main(int argc, char *argv[])
{
    bool do_plot_in_terminal = false;
    bool do_use_cpp_implementation = true;
    bool do_use_filter = false;

    // set up chirp generator
    float Ts = 1.0e-2f;
    float f0 = 0.2f;
    float f1 = 0.99f/2.0f/Ts;
    uint32_t N = 10e3;
    CHIRP *chirp_cpp = new CHIRP();
    chirp_cpp->init(f0, f1, N, Ts);

    chirp_t *chirp_c = new chirp_t;
    chirpInit(chirp_c, f0, f1, N, Ts);

    float f_cut = 20.0f;
    // biquadFilter_t *filter_c = new biquadFilter_t;
    // biquadFilterInitLPF(filter_c, f_cut, (uint32_t)(Ts * 1.0e6f));
    pt1TustinFilter_t *filter_c = new pt1TustinFilter_t;
    pt1TustinFilterInit(filter_c, f_cut, (uint32_t)(Ts * 1.0e6f));
    // pt1TustinFilter_t *filter_c2 = new pt1TustinFilter_t;
    // pt1TustinFilterInit(filter_c2, f_cut, (uint32_t)(Ts * 1.0e6f));
    // pt2Filter_t *filter_c = new pt2Filter_t;
    // pt2FilterInit(filter_c, pt2FilterGain(f_cut, Ts));

    uint32_t cntr = 0;

    // open file for writing
    std::ofstream datafile("output/chirp_meas_01.txt");

    bool main_execute = true;
    while (main_execute) {
        cntr++; 
        if (do_use_cpp_implementation) {
            if (chirp_cpp->update()) {
                float exc = chirp_cpp->exc();
                float excf = 0.0f;
                if (do_use_filter) {
                    // float excf = biquadFilterApply(filter_c, exc);
                    float excf = pt1TustinFilterApply(filter_c, exc);
                    // excf = pt1TustinFilterApply(filter_c2, excf);
                    // float excf = pt2FilterApply(filter_c, exc);
                }
                if (do_plot_in_terminal) {
                    std::cout << cntr << ", " << excf << ", " << exc  - excf << ", " << chirp_cpp->sinarg() << ", " << chirp_cpp->fchirp() << std::endl;
                }
                datafile << cntr << ", " << excf << ", " << exc  - excf << ", " << chirp_cpp->sinarg() << ", " << chirp_cpp->fchirp() << std::endl;
            } else {
                std::cout << "---   chirp finished" << std::endl;
                main_execute = false;
            }
        } else {
            if (chirpUpdate(chirp_c)) {
                float exc = chirp_c->exc;
                float excf = 0.0f;
                if (do_use_filter) {
                    // float excf = biquadFilterApply(filter_c, exc);
                    float excf = pt1TustinFilterApply(filter_c, exc);
                    // excf = pt1TustinFilterApply(filter_c2, excf);
                    // float excf = pt2FilterApply(filter_c, exc);
                }
                if (do_plot_in_terminal) {
                    std::cout << cntr << ", " << excf << ", " << exc  - excf << ", " << chirp_c->sinarg << ", " << chirp_c->fchirp << std::endl;
                }
                datafile << cntr << ", " << excf << ", " << exc  - excf << ", " << chirp_c->sinarg << ", " << chirp_c->fchirp << std::endl;
            } else {
                std::cout << "---   chirp finished" << std::endl;
                main_execute = false;
            }
        }
    }

    datafile.close();

    return 0;
}
