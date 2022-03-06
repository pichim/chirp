#include <iostream>
#include <thread>
#include <atomic>
#include <fstream>

#include "CHIRP.h"

// set up chirp generator
float Ts = 0.01f;
float f0 = 0.2f;
float f1 = 0.99f/2.0f/Ts;
int Nchirp = 1000;
CHIRP chirp = CHIRP(f0, f1, Nchirp, Ts);
int cntr = 0;

// open file for writing
std::ofstream datafile;

// atomic bool for thread control
std::atomic<bool> thread_execute = true;

void thread_fcn()
{
    if (thread_execute) {
        while(thread_execute) {

            float sinarg(0.0f);
            float fchirp(0.0f);
            float exc = chirp.update(sinarg, fchirp);
            if(cntr++ < Nchirp) {
                std::cout << cntr << ", " << exc << ", " << sinarg << ", " << fchirp << std::endl;
                datafile << cntr << ", " << exc << ", " << sinarg << ", " << fchirp << std::endl;
            } else {
                std::cout << "---   chirp finished" << std::endl;
                thread_execute = false;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    } else {
        std::this_thread::yield();
    }   
}

int main(int argc, char *argv[])
{
    datafile.open("chirp_meas_01.txt");

    // start thread
    std::thread chirp_thread(&thread_fcn);
    
    // aboard
    //std::cout << "---   hit Enter to terminate ---\n" << std::endl;
    //std::cin.ignore();
    //thread_execute = false;

    // join thread
    chirp_thread.join();

    datafile.close();
    
    return 0;
}
