const int n_rows = 1280;
const int n_cols = 320;

const int N_rows = n_rows / 32;
const int N_cols = n_cols / 32;

const int N = 64;

void launch_gpu_matrix_scale(float* A_cpu, float* B_cpu, float* D_cpu) {
    // Pointers for memory on the GPU
    float* A_gpu = new float[N*N_cols]; 
    float* B_gpu = new float[N*N_cols]; 
    float* D_gpu = new float[N_cols]; 

    // Allocate memory on the GPU
    cudaMalloc(&A_gpu, (N*N_cols)*sizeof(float));
    cudaMalloc(&B_gpu, (N*N_cols)*sizeof(float));
    cudaMalloc(&D_gpu, N_cols*sizeof(float));

    // Copy memory to the GPU
    cudaMemcpy(A_gpu, A_cpu, (N*N_cols), cudaMemCopyHostToDeivce);
    cudaMemcpy(B_gpu, B_cpu, (N*N_cols), cudaMemCopyHostToDeivce);
    cudaMemcpy(D_gpu, D_cpu, N_cols, cudaMemCopyHostToDeivce);

    // Compute how much shared memory is needed
    int D_sharedMem = n_cols*sizeof(float);

    // Compute the size of the CUDA grid (i.e. the number of blocks in each dim)
    dim3 threadsPerBlock(N); 
    dim3 num_blocks(n_rows/N, N_cols);

    // Invoke the CUDA kernel
    matrix_scale_wrapped<<<num_blocks, threadsPerBlock, D_sharedMem>>>(A_gpu, B_gpu, D_gpu, n_rows, n_cols, N);

    // Copy memory back to the CPU
    cudaMemcpy(A_gpu, A_cpu, (N*N_cols), cudaMemCopyDeviceToHost);
    cudaMemcpy(B_gpu, B_cpu, (N*N_cols), cudaMemCopyDeviceToHost);
    cudaMemcpy(D_gpu, D_cpu, N_cols, cudaMemCopyDeviceToHost);

    // Free memory on the GPU
    cudaFree(A_gpu);
    cudaFree(B_gpu);
    cudaFree(D_gpu);
}

__global__ matrix_scale_wrapped(float* A, float* B, float* D, const int n_rows, const int n_cols, const int N) {
    
    // Shared memory for D
    extern __shared__ float D_shared[];
   
    // Thread ID 
    int I = blockIdx.x;

    // Block ID
    int Dx = blockDim.x;
    int Dy = blockDim.y;

    // Perform Computation
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N_cols; j++){
            if (i == j){
                B[i + j*N_cols] = A[i + j*N_cols]*D[j];
            } else {
                B[i + j*N_cols] = 0.0;
            }
        }
    }

}

int main() {
    // Pointers for the matrices/arrays
    float* A;
    float* B;
    float* D;

    // Allocate the arrays
    A = new float[n_rows*n_cols];
    B = new float[n_rows*n_cols];
    D = new float[n_cols];

    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_cols; j++) {
            A[j + i*n_cols] = 1;
        }
    }

    for(int j = 0; j < n_cols; j++)
        D[j] = j;

    launch_gpu_matrix_scale(A, B, D);

    // Check B
    int correct = 0;
    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_cols; j++) {
            if (B[j + i *n_cols] != j)
                correct = -1;
        }
    }

    delete[] A;
    delete[] B;
    delete[] D;
    
    return correct;
}
