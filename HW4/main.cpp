#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <mpi.h>

#include "halo_exchange.h"

using std::getenv;
using std::printf;
using std::stoi; 

int main() {
    MPI_Init(NULL, NULL);

    int* data = NULL;
    const int NX = stoi(getenv("NX"));
    const int NY = stoi(getenv("NY"));
    const int HALO_RADIUS = stoi(getenv("HALO_RADIUS"));

    int N_ranks;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &N_ranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if( rank == 0 ) {
        if( N_ranks != NX * NY ) {
            printf("NX*NY != the number of ranks! Ending program...\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        if( nx % NX != 0 ) {
            printf("NX does not evenly divide nx. Rerun the program with a new NX.\n");
            MPI_Abort(MPI_COMM_WORLD, 2);
        }
        if( ny % NY != 0 ) {
            printf("NY does not evenly divide ny. Rerun the program with a new NY.\n");
            MPI_Abort(MPI_COMM_WORLD, 3);
        }
    }

    // Allocate this rank's memory
    const int nx_local = nx / NX;
    const int ny_local = ny / NY;

    
    int nx_total_local = nx_local + 2*HALO_RADIUS;
    int ny_total_local = ny_local + 2*HALO_RADIUS;
    int nx_total_global = NX*nx_total_local;
    int ny_total_global = NY*ny_total_local;

    int n_total_local = nx_total_local * ny_total_local;
    int n_total_global = NX*NY*n_total_local;
    data = new int[n_total_local];
    if(data == NULL) {
        printf("Rank %d could not allocate data. Ending program.\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 4);
    }


    process_block(data, nx_local, ny_local, NX, NY, HALO_RADIUS);
    MPI_Barrier(MPI_COMM_WORLD);

    // Have rank 0 print the results
    int* data_all = NULL;
    if( rank == 0 ) {
        data_all = new int[n_total_global];
    }

    MPI_Gather(data, n_total_local, MPI_INT, data_all, n_total_local, MPI_INT, 0, MPI_COMM_WORLD);

    // Print all the data to inspect
    if(rank == 0) {
        int I;
        int J;
        int block;

        int i_local;
        int j_local;

        for(int j = 0; j < ny_total_global; j++) {
            if( j > 0 && j % ny_total_local == 0 ) 
                printf("\n\n");
            for(int i = 0; i < nx_total_global; i++) {
                if ( i > 0 && i % nx_total_local == 0)
                    printf("   ");
                I = i / nx_total_local;
                J = j / ny_total_local;
                block = J*NX + I;

                i_local = i % nx_total_local;
                j_local = j % ny_total_local;

                printf(" %d", data_all[block*n_total_local + j_local*nx_total_local + i_local]);
            }
                printf("\n");
        }
        delete[] data_all;
    }
    
    delete[] data;
    MPI_Finalize();
    return 0; 
}
