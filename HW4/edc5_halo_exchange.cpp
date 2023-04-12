#include "halo_exchange.h"

void pack(int* memory, int* buffer, int nx_local, int ny_local, int halo_radius){
    // Initialize variables
    int j_stride = nx_local + 2*halo_radius;
    int count = 0;

    // loop to fill buffer
    for (int j = 0; j < ny_local; j++){
        for (int i = 0; i < halo_radius; i++){
            buffer[count] = memory[cartesian2flat(i,j,j_stride)]; 
            count++;
        }
    }
}

void unpack(int* memory, int* buffer, int nx_local, int ny_local, int halo_radius){
    // Initialize variables
    int j_stride = nx_local + 2*halo_radius;
    int count = 0;
    
    // loop to unpack buffer
    for (int j = 0; j < ny_local; j++){
        for (int i = 0; i < halo_radius; i++){
            memory[cartesian2flat(i,j,j_stride)] = buffer[count];
            count++;
        }
    }
}

void process_block(int* memory, int nx_local, int ny_local, int Nx, int Ny, int halo_radius){
    // Initialize local variables
    int rank_src;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_src);
    int j_stride = nx_local + 2*halo_radius;
    int tag_x = 1;
    int tag_y = 0;
    MPI_Request request_send_x;
    MPI_Request request_recv_x;
    MPI_Request request_send_y;
    MPI_Request request_recv_y;

    // Compute inner halo
    for (int j = halo_radius; j < 2*halo_radius; j++){
        for (int i = halo_radius; i < (nx_local + halo_radius); i++){
            memory[cartesian2flat(i,j,j_stride)] = rank_src; 
        }
    }

    for (int j = 2*halo_radius; j < ny_local; j++){
        for (int i = halo_radius; i < 2*halo_radius; i++){
            memory[cartesian2flat(i,j,j_stride)] = rank_src; 
        }
        for (int i = nx_local; i < (nx_local + 2*halo_radius); i++){
            memory[cartesian2flat(i,j,j_stride)] = rank_src; 
        }
    }
    
    for (int j = ny_local; j < (ny_local + halo_radius); j++){
        for (int i = halo_radius; i < (nx_local + halo_radius); i++){
            memory[cartesian2flat(i,j,j_stride)] = rank_src; 
        }
    }
  
    // Post sends and receives inner and outer halo rows
    if (Ny > 1) {
        for (int j = 0; j < 2; j++){
           
            // Compute the destination rank
            int rank_dest;
            if (rank_src < Nx){
                rank_dest = rank_src + (Ny - 1)*Nx;
            } else if (rank_src >= (Ny - 1)*Nx){
                rank_dest = rank_src - (Ny - 1)*Nx;
            } else {
                rank_dest = rank_src + ((-1)^j)*Nx;
            }
           
            // Post an asynchronous send of the inner halo
            request_send_x = MPI_REQUEST_NULL; 
            int* buffer_send = &memory[cartesian2flat(0, halo_radius + j*(ny_local - halo_radius), j_stride)];
            MPI_Isend(buffer_send, (nx_local + 2*halo_radius)*halo_radius, MPI_INT, rank_dest, tag_x, MPI_COMM_WORLD, &request_send_x);
            
            // Post an asynchronous receive of the outer halo 
            request_recv_x = MPI_REQUEST_NULL; 
            int* buffer_recv = &memory[cartesian2flat(0, j*(ny_local + halo_radius), j_stride)]; 
            MPI_Irecv(buffer_recv, (nx_local + 2*halo_radius)*halo_radius, MPI_INT, rank_dest, tag_x, MPI_COMM_WORLD, &request_recv_x);
        }  
    }

    // Post sends and receives inner and outer halo columns
    int** buffer_recv = NULL; 
    int** memory_unpack = NULL; 
    if (Nx > 1){
        
        // Allocate receive buffers and memory unpacking addresses
        buffer_recv = new int* [2]; 
        memory_unpack = new int* [2]; 
        
        for (int i = 0; i < 2; i++){
            
            // Compute the destination rank
            int rank_dest;
            if (rank_src % Nx == 0){
                rank_dest = rank_src + Nx - 1;
            } else if ((rank_src + 1) % Nx == 0){
                rank_dest = rank_src + 1 - Nx;
            } else {
                rank_dest = rank_src + (-1)^(i+1);
            }
            
            // Pack send data and post an asynchronous send of the inner halo 
            request_send_y = MPI_REQUEST_NULL; 
            int* buffer_send = new int[ny_local*halo_radius]; 
            int* memory_pack = &memory[cartesian2flat(halo_radius + i*(nx_local - halo_radius), halo_radius, j_stride)];
            pack(memory_pack, buffer_send, nx_local, ny_local, halo_radius);
            MPI_Isend(buffer_send, ny_local*halo_radius, MPI_INT, rank_dest, tag_y, MPI_COMM_WORLD, &request_send_y);
            delete[] buffer_send;
            
            // Post an asynchronous receive of the outer halo 
            request_recv_y = MPI_REQUEST_NULL; 
            buffer_recv[i] = new int[ny_local*halo_radius];
            memory_unpack[i] = &memory[cartesian2flat(i*(nx_local + halo_radius), halo_radius, j_stride)];
            MPI_Irecv(buffer_recv[i], ny_local*halo_radius, MPI_INT, rank_dest, tag_y, MPI_COMM_WORLD, &request_recv_y);
        }
    }
    
    // Compute the inner region 
    for (int j = 2*halo_radius; j < ny_local; j++){
        for (int i = 2*halo_radius; i < nx_local; i++){
            memory[cartesian2flat(i,j,j_stride)] = rank_src; 
        }
    }
    
    // Synchronize block processing
    if (Ny > 1){
        MPI_Wait(&request_send_x, MPI_STATUS_IGNORE);
        MPI_Wait(&request_recv_x, MPI_STATUS_IGNORE);
    }
    if (Nx > 1){
        MPI_Wait(&request_send_y, MPI_STATUS_IGNORE);
        MPI_Wait(&request_recv_y, MPI_STATUS_IGNORE);

        // Unpack the outer halo once the receive communication is complete
        for (int i = 0; i < 2; i++){
            unpack(memory_unpack[i], buffer_recv[i], nx_local, ny_local, halo_radius);
            delete[] buffer_recv[i];
        }
        delete[] buffer_recv;
        delete[] memory_unpack;
    }

}
