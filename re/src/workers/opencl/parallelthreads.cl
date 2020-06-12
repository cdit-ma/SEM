

__kernel void runParallel(long num_ops) {
    volatile long accumulator=0;
    //__local long accumulator[4096];
    // #pragma UNROLL 1
    for (long i=0; i<num_ops; i++) {
        //mem_fence(CLK_LOCAL_MEM_FENCE);
        accumulator/*[get_local_id(0)]*/++;
    }
}