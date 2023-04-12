#include <cmath>
#include <omp.h>

#include "wavefront.h"

void process_block(double* data, int I, int J, int nx, int ny, int Nx, int Ny){
    
    //initalize equation constants, nodes per block 
    const double PI = 3.14159265358979323846;
    int nb_x = ceil((double)(nx + 1)/(double)Nx);
    int nb_y = ceil((double)(ny + 1)/(double)Ny);
    int x_end = (I+1)*nb_x;
    int y_end = (J+1)*nb_y;
    int x_ele = nb_x;
    int y_ele = nb_y;
    
    // check is block extends out of domain
    if (x_end > nx + 1) {
        x_ele = nx + 1 - I*nb_x; 
    }
    if (y_end > ny + 1) {
        y_ele = ny + 1 - J*nb_y;
    } 
    
    // compute each node in the block
    for (int i = 0; i < x_ele; i++){
        for (int j = 0; j < y_ele; j++){
            if (I*nb_x + i == 0) {
                double y_coord = ((double)(J*nb_y + j))/((double)(ny + 1));
                data[cartesian2flat(I*nb_x + i, J*nb_y + j, ny+1)] = sin(2.0*PI*y_coord); 
            } else if (J*nb_y + j == 0) {
                double x_coord = ((double)(I*nb_x + i))/((double)(nx + 1));
                data[cartesian2flat(I*nb_x + i, J*nb_y + j, ny+1)] = sin(2.0*PI*x_coord); 
            } else {
                data[cartesian2flat(I*nb_x + i, J*nb_y + j, ny+1)] = Cx*data[cartesian2flat(I*nb_x + i - 1, J*nb_y + j, ny+1)] + Cy*data[cartesian2flat(I*nb_x + i, J*nb_y + j - 1, ny+1)]; 
            }
        }
    }
}

void wavefront520(double* data, int nx, int ny, int Nx, int Ny){
    
    // initialize while loop variables
    const int num_wave = Nx + Ny - 1;
    int T_num;
    int T_id;
    int I;
    int J;
    int w = 0;
    int J_lb = 0;
    int I_lb = 0;
    int I_ub = 0;

    #pragma omp parallel shared(data, nx, ny, Nx, Ny, num_wave, T_num) private(T_id, I, J) firstprivate(w, I_lb, I_ub, J_lb)
    {
        // get thread information
        T_num = omp_get_num_threads(); 
        T_id = omp_get_thread_num();

        // loop over spin-up and fully parallel region
        while (w < (num_wave - T_num + 1)) {
            I = I_ub - T_id;
            J = J_lb + T_id; 
            
            // check if in spin-up region
            if ((w < T_num - 1) && ((I >= I_lb) && (I <= I_ub) && (J >= J_lb) && (J <= w - I_lb))) {
                process_block(data, I, J, nx, ny, Nx, Ny);
                w++;
                I_ub++;
                I = I_ub - T_id;
            } else {
                // computer over a wave until indices are invalid
                while ((I >= I_lb) && (I <= I_ub) && (J >= J_lb) && (J <= w - I_lb)){
                    process_block(data, I, J, nx, ny, Nx, Ny);
                    I -= T_num;
                    J += T_num;
                } 
                w++;
                if (w > (Nx - 1)){
                    I_ub = (Nx - 1);
                    J_lb = (w - 1) - (Nx - 2);
                } else {
                    I_ub = w;
                }
                if (w > (Ny - 1)){
                    I_lb = (w - 1) - (Ny - 2);
                }
            }
            #pragma omp barrier
        }

        //loop over spin down region
        #pragma omp barrier
        for (w = (num_wave - T_num + 1); w < num_wave; w++){
            #pragma omp for private(J)
            for (I = (Nx - 1); I > T_num - (num_wave - w + 1); I--){
                J = w - I;
                process_block(data, I, J, nx, ny, Nx, Ny);
            }
        }
    }
}

void wavefront420(double* data, int nx, int ny, int Nx){
}
