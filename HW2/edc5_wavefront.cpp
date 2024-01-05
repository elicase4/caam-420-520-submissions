#include <cmath>
#include <iostream>
#include <omp.h>
#include "wavefront.h"

// for debug
#include <sstream>
#include <iomanip>

void process_block(double* data, int I, int J, int nx, int ny, int Nx, int Ny){
                   
    // Initalize equation constants, nodes per block 
    int nb_x = ceil((nx + 1)/Nx);
    int nb_y = ceil((ny + 1)/Ny);
    int x_end = (I+1)*nb_x;
    int y_end = (J+1)*nb_y;
    int x_ele = nb_x;
    int y_ele = nb_y;
    
    // Check if block extends out of domain
    if (x_end >= nx) {
        x_ele = (nx + 1) - I*nb_x; 
    }

    if (y_end >= ny) {
        y_ele = (ny + 1) - J*nb_y;
    }

    // Compute each node in the block
    for (int i = 0; i < x_ele; i++){
        for (int j = 0; j < y_ele; j++){
            
            // Compute the 2d indices 
            int x_idx = (I*nb_x + i);
            int y_idx = (J*nb_y + j);

            // X inlet boundary condition  
            if (x_idx == 0) {
                double y_coord = (double) (y_idx + 1)/(ny + 1);
                data[cartesian2flat(x_idx, y_idx, (ny + 1))] = sin(2.0*M_PI*y_coord); 
            
            // Y inlet boundary condition  
            } else if (y_idx == 0) {
                double x_coord = (double) (x_idx + 1)/(nx + 1);
                data[cartesian2flat(x_idx, y_idx, (ny + 1))] = sin(2.0*M_PI*x_coord); 
            
            // Interior FD stencil 
            } else {
                data[cartesian2flat(x_idx, y_idx, (ny + 1))] = Cx*data[cartesian2flat(x_idx - 1, y_idx, (ny + 1))] + Cy*data[cartesian2flat(x_idx, y_idx - 1, (ny + 1))]; 
            }
        }
    }
}

void wavefront520(double* data, int nx, int ny, int Nx, int Ny){
    
    // Initialize while loop variables
    const int num_wave = Nx + Ny - 1;
    int T_num, T_id, I, J, I_lb, J_lb, I_ub, J_ub;
    int w = 0;
    
    // for debug    
    std::stringstream msg;

    #pragma omp parallel shared(data, nx, ny, Nx, Ny, num_wave, T_num) private(T_id, I, J, I_lb, J_lb, I_ub, J_ub, msg) firstprivate(w)
    {
        
        // Get thread information
        T_num = omp_get_num_threads(); 
        T_id = omp_get_thread_num();
            
        // Map thread ID to blocks
        I = -T_id;
        J = T_id;

        // Loop over the wavefronts
        while (w < num_wave){
            
            // Compute bounds
            I_lb = std::max(0, w - (Nx - 1));
            J_lb = std::max(0, w - (Ny - 1));
            I_ub = std::min(w, Nx - 1);
            J_ub = std::min(w, Ny - 1);

            if (w < T_num){

                if ((I >= I_lb) && (I <= I_ub) && (J >= J_lb) && (J <= J_ub)){
                    
                    // debug print
                    msg << "T_id = " << T_id << " | (I, J) = (" << I << "," << J << ") | w = " << w << " | (I_lb, I_ub, J_lb, J_ub) = (" << I_lb << "," << I_ub << "," << J_lb << "," << J_ub << ")\n";
                    std::cout << msg.str();

                    process_block(data, I, J, nx, ny, Nx, Ny);
                }
                w++;
                I++;

                #pragma omp barrier
                
            } else if ((w >= T_num) && (w < (num_wave - T_num))){
                  
                while ((I >= I_lb) && (I <= I_ub) && (J >= J_lb) && (J <= J_ub)){
                    
                    // debug print
                    msg << "T_id = " << T_id << " | (I, J) = (" << I << "," << J << ") | w = " << w << " | (I_lb, I_ub, J_lb, J_ub) = (" << I_lb << "," << I_ub << "," << J_lb << "," << J_ub << ")\n";
                    std::cout << msg.str();

                    process_block(data, I, J, nx, ny, Nx, Ny);
                    I -= T_num;
                    J += T_num; 
                }
                
                w++;
                I += Nx;
                J -= Ny - 1;
                    
                #pragma omp barrier
                
            } else {
                
                if ((I >= I_lb) && (I <= I_ub) && (J >= J_lb) && (J <= J_ub)){
                    
                    // debug print
                    msg << "T_id = " << T_id << " | (I, J) = (" << I << "," << J << ") | w = " << w << " | (I_lb, I_ub, J_lb, J_ub) = (" << I_lb << "," << I_ub << "," << J_lb << "," << J_ub << ")\n";
                    std::cout << msg.str();

                    process_block(data, I, J, nx, ny, Nx, Ny);
                }
                
                w++;
                J++;

                // thread mapping is missing in spin-down
                
                #pragma omp barrier
                
            }
        } 
    }


}

void wavefront420(double* data, int nx, int ny, int Nx){
}
