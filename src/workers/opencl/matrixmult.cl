// Matrix multiplication without any optimizations, assumes square matrices
// and that work-item dimensions equal matrix dimensions
__kernel void matrixMultSimple( __global float* in1,
                                __global float* in2,
                                __global float* out ) {
    uint eX = get_global_id(0);
    uint eY = get_global_id(1);
    

    uint rowLength = get_global_size(0);

    uint eYrowOffset = eY*rowLength;

    float eSum = 0;
    for (uint step=0; step < rowLength; step++) {
        eSum += in1[step + eYrowOffset] * in2[eX + step*rowLength];
    }

    out[eX + eYrowOffset] = eSum;
}

// Matrix multiplication using shared memory blocks, assumes square matrices
// and that work-item dimensions equal matrix dimensions
__kernel void matrixMultAdv( __global float* in1,
                                __global float* in2,
                                __global float* out,
                                __local float* localA,
                                __local float* localB ) {
    uint eX = get_global_id(0);
    uint eY = get_global_id(1);


    uint rowLength = get_global_size(0);
    uint blockLength = get_local_size(0);

    uint eYrowOffset = eY*rowLength;

    float eSum = 0;
    for (uint block = 0; block<get_num_groups(0); block++) {

        localA[get_local_id(0) + blockLength*get_local_id(1)] = in1[block*blockLength + get_local_id(0) + eYrowOffset];
        localB[get_local_id(0) + blockLength*get_local_id(1)] = in2[eX + (block*blockLength + get_local_id(1))*rowLength];
        
        barrier(CLK_LOCAL_MEM_FENCE);

        for (uint step=0; step<blockLength; step++) {
            eSum += localA[step + blockLength*get_local_id(1)] * localB[get_local_id(0) + blockLength*step];
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }
    
    out[eX + eYrowOffset] = eSum;
}

// Matrix multiplication using shared memory blocks, multiplies an
// MxK matrix with a KxN matrix to produce a MxN
__kernel void matrixMultFull( __global float* in1,
                                __global float* in2,
                                __global float* out,
                                uint M,
                                uint K,
                                uint N,
                                __local float* localA,
                                __local float* localB ) {
    uint eRow = get_global_id(0);
    uint eCol = get_global_id(1);

    uint lRow = get_local_id(0);
    uint lCol = get_local_id(1);

    uint blockLength = get_local_size(0);

    float eSum = 0;
    for (uint block = 0; block<(K-1)/blockLength+1; block++) {

        //if (eX < K && eY < M) {
        if (block*blockLength + lCol < K && eRow < M) {
            localA[lCol + blockLength*lRow] = in1[(block*blockLength + lCol) + eRow*K];
        }
        //if (eX < N && eY < K) {
        if (eCol < N && block*blockLength + lRow < K) {
            localB[lCol + blockLength*lRow] = in2[eCol + (block*blockLength + lRow)*N];
        }
        
        barrier(CLK_LOCAL_MEM_FENCE);
        uint maxSteps = min(blockLength, K - blockLength*block);
        
        for (uint step=0; step<maxSteps; step++) {
            eSum += localA[step + blockLength*lRow] * localB[lCol + blockLength*step]; 
            //eSum = fma(localA[step + blockLength*lRow], localB[lCol + blockLength*step], eSum);  
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }
    
    if (eRow < M && eCol < N) {
        out[eCol + eRow*N] = eSum;
    }
}
