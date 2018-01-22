/*
__kernel void emptyTestKernel() {
 //blank
}

__kernel void signatureTestKernel(
    int intTestVariable,
    __local float4* float4TestBuffer,
    __global double2* double2TestBuffer) {
    //blank
}*/

__kernel void dataPassthroughTest(
    __global float* inBuffer,
    __global float* outBuffer) {
    
    uint i = get_global_id(0);
    outBuffer[i] = inBuffer[i];
    //outBuffer[i] = inData;
}