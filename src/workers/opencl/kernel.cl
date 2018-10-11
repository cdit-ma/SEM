
__kernel void dataPassthroughTest(
    __global float* inBuffer,
    __global float* outBuffer) {
    
    uint i = get_global_id(0);
    outBuffer[i] = inBuffer[i];
    //outBuffer[i] = inData;
}