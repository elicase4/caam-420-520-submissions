#ifndef HALO_EXCHANGE_H
#define HALO_EXCHANGE_H
#include <mpi.h>


const int nx = 128;
const int ny = 128;

inline int cartesian2flat(int i, int j, int j_stride) {
    return j*j_stride + i;
}


// For packing and unpacking buffers (note you must allocate the buffers yourself though;
// you should be able to use your allocate_memory function) before/after communication.
void pack(int* memory, int* buffer, int nx_local, int ny_local, int halo_radius);   // writes from memory to buffer
void unpack(int* memory, int* buffer, int nx_local, int ny_local, int halo_radius); // writes from buffer to memory

void process_block(int* memory, int nx_local, int ny_local, int Nx, int Ny, int halo_radius);

#endif
