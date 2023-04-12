#include "prob2.h"

#include <mpi.h>


int main() {
    MPI_Init(NULL, NULL);

    group_exchange();

    MPI_Finalize();
    return 0;
}
