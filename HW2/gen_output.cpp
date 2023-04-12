#include <chrono>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

#include <omp.h>

#include "wavefront.h"

using std::printf;
using std::scanf;
using std::chrono::steady_clock;
using std::micro;
using std::stoi;

const int CAAM_420 = 420;
const int CAAM_520 = 520;

void write_result(double* data, const char filename[]);

int main(int argc, char* argv[]) {
    // Parse arguments
    if (argc < 3 ) {
        printf("Not enough arguments supplied; exiting program...\n");
        return -1;
    } 
    
    int choice = stoi(argv[1], NULL);
    int Nx = 0;
    int Ny = 0;
    if (argc == 3 && choice == CAAM_420) {
        Nx = stoi(argv[2], NULL);
    } else if (argc == 4 && choice == CAAM_520) {
        Nx = stoi(argv[2], NULL);
        Ny = stoi(argv[3], NULL);
    } else {
        printf("Incorrect number of arguments supplied for option %d; exiting program...\n", choice);
        return -1;
    }

    // Allocate memory
    double* data = NULL;
    data = new double[(ny+1)*(nx+1)];
    if (data == NULL) {
        printf("Could not allocate enough memory for data; exiting program... \n");
        return -2;
    }

    // Call the appropriate function
    if (choice == CAAM_420)
        wavefront420(data, nx, ny, Nx);
    else if (choice == CAAM_520)
        wavefront520(data, nx, ny, Nx, Ny);
    else {
        printf("Invalid choice entered; exiting program...\n");
        return -3;
    }

    const char output[] = "output.txt";

    write_result(data, output);

    return 0;
}

void write_result(double* data, const char filename[]){
    std::ofstream file(filename);
    for (int i = 0; i < nx + 1; i++) {
        for (int j = 0; j < ny + 1; j++) {
            file << (float) data[cartesian2flat(i,j,ny+1)] << " ";
        }
        file << "\n";
    }
    file.close();
}
