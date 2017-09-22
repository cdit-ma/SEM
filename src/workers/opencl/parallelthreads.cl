__kernel void runParallel(__global int* in_buffer, int num_ops) {
    for (int i=0; i<num_ops; i++) {
        in_buffer[get_local_id(0)]++;
    }
}