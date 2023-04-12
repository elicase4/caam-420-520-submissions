#include "prob2.h"

#include <mpi.h>

void group_exchange() {
    int* data_ptr = new int;
    int* rank_num_ptr = new int;
    int* num_ranks_ptr = new int;
    
    // find the size of the communicator and the rank number
    MPI_Comm_rank(MPI_COMM_WORLD, rank_num_ptr);
    MPI_Comm_size(MPI_COMM_WORLD, num_ranks_ptr);

    // loop over all ranks in the communicator to send and recieve messages from the proper rank
    for (int i = 0; i < num_ranks; i++) {
        if (rank_num == i) {
            data = i;
            MPI_Send(data_ptr, 1, MPI_INT, (i+1) % num_ranks, 0, MPI_COMM_WORLD);
            printf("Process %d sent number %d to process %d\n", i, data, (i+1) % num_ranks);
        } else if (rank_num == (i+1) % num_ranks) {
            MPI_Recv(data_ptr, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process %d received number %d from process %d\n", (i+1) % num_ranks, data, i);
        }
    }
}
