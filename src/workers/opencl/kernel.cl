
__kernel void emptyTestKernel() {
 //blank
}

__kernel void signatureTestKernel(
    int intTestVariable,
    __local float4* float4TestBuffer,
    __global double2* double2TestBuffer) {
    //blank
}

__kernel void dataPassthroughTest(
    __global float* inBuffer,
    __global float* outBuffer) {
    
    uint i = get_global_id(0);
    outBuffer[i] = inBuffer[i];
    //outBuffer[i] = inData;
}

__kernel void runParallel(/*__global int* in_buffer,*/ int num_ops) {
    for (int i=0; i<num_ops; i++) {
        //in_buffer[get_local_id(0)]++;
    }
}