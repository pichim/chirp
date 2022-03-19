#include <iostream>
#include <fstream>

#include "chirp_cpp.h"
#include "chirp_c.h"
#include "filter.h"

int main(int argc, char *argv[])
{
    bool do_plot_in_terminal = false;
    bool do_use_cpp_implementation = false; // 19.02.2022 stopped also taking cpp implementation further
    bool do_use_filter = true;
    bool do_use_exp_chirp = false;

    // set up chirp generator
    float Ts = 1.0f / 8.0e3f;
    float f0 = 0.2f;
    float f1 = 0.99f/2.0f/Ts;
    uint32_t N = 20 * 8e3;
    float alpha = 4.0;

    CHIRP *chirp_cpp = new CHIRP();
    chirp_cpp->init(f0, f1, N, Ts);

    // chirp_t *chirp_c = new chirp_t;
    // chirpInit(chirp_c, f0, f1, N, Ts);
    chirp_t chirp_c;
    if (do_use_exp_chirp) {
        chirpInitExp(&chirp_c, f0, f1, N, Ts);
    } else {
        chirpInitAlpha(&chirp_c, f0, f1, N, Ts, alpha);
    }    

    float f_cut = 10.0f;
    // biquadFilter_t *filter_c = new biquadFilter_t;
    // biquadFilterInitLPF(filter_c, f_cut, (uint32_t)(Ts * 1.0e6f));
    pt1TustinFilter_t *filter_c = new pt1TustinFilter_t;
    pt1TustinFilterInit(filter_c, f_cut, Ts);
    pt1TustinFilter_t *filter_c2 = new pt1TustinFilter_t;
    pt1TustinFilterInit(filter_c2, 100.0f * f_cut, Ts);
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
                    // excf = biquadFilterApply(filter_c, exc);
                    excf = pt1TustinFilterApply(filter_c, exc);
                    excf = pt1TustinFilterApply(filter_c2, excf);
                    // excf = pt2FilterApply(filter_c, exc);
                }
                if (do_plot_in_terminal) {
                    std::cout << cntr << ", " << exc << ", " << excf << ", " << chirp_cpp->sinarg() << ", " << chirp_cpp->fchirp() << std::endl;
                }
                datafile << cntr << ", " << exc << ", " << excf << ", " << chirp_cpp->sinarg() << ", " << chirp_cpp->fchirp() << std::endl;
            } else {
                std::cout << "---   chirp finished" << std::endl;
                main_execute = false;
            }
        } else {
            if (chirpUpdate(&chirp_c)) {
                float exc = chirp_c.exc;
                float excf = 0.0f;
                if (do_use_filter) {
                    // excf = biquadFilterApply(filter_c, exc);
                    excf = pt1TustinFilterApply(filter_c, exc);
                    excf = pt1TustinFilterApply(filter_c2, excf);
                    // excf = pt2FilterApply(filter_c, exc);
                }
                if (do_plot_in_terminal) {
                    std::cout << cntr << ", " << exc << ", " << excf << ", " << chirp_c.sinarg << ", " << chirp_c.fchirp << std::endl;
                }
                datafile << cntr << ", " << exc << ", " << excf << ", " << chirp_c.sinarg << ", " << chirp_c.fchirp << std::endl;
            } else {
                std::cout << "---   chirp finished" << std::endl;
                main_execute = false;
            }
        }
    }

    datafile.close();

    return 0;
}
