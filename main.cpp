#include <iostream>
#include <pthread.h>
#include <fstream>

#include "CHIRP.h"
#include "chirp_c.h"

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
    // chirpOptimizeParametersForEndFrequency(chirp2);
    // chirpReset(chirp2);

    uint32_t cntr = 0;

    // open file for writing
    std::ofstream datafile("chirp_meas_01.txt");

    bool main_execute = true;
    while (main_execute)
    {
        cntr++;
        /*if (chirp->update())
        {
            std::cout << cntr << ", " << chirp->exc() << ", " << chirp->sinarg() << ", " << chirp->fchirp() << std::endl;
            datafile << cntr << ", " << chirp->exc() << ", " << chirp->sinarg() << ", " << chirp->fchirp() << std::endl;
        }*/
        if (chirpUpdate(chirp2))
        {
            std::cout << cntr << ", " << chirp2->exc << ", " << chirp2->sinarg << ", " << chirp2->fchirp << std::endl;
            datafile << cntr << ", " << chirp2->exc << ", " << chirp2->sinarg << ", " << chirp2->fchirp << std::endl;
        }
        else
        {
            std::cout << "---   chirp finished" << std::endl;
            main_execute = false;
        }
    }

    datafile.close();

    return 0;
}
