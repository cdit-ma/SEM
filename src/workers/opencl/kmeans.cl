
__kernel void classifyPoints(__global float4* points,
                                __global int* classes,
                                __global float4* centers,
                                uint numCenters) {
    uint index = get_global_id(0);
    float minDist = FLT_MAX;
    float currentDist;
    
    for (uint i=0; i<numCenters; i++) {
        currentDist = distance(points[index], centers[i]);
        if (currentDist < minDist) {
            minDist = currentDist;
            classes[index] = i;
        }
    }
}

/**
 * Performs a two-phase reduction. In the first phase each work-item accesses points in a strided sequential
 * fashion until all points have been processed, giving each work-item a local total while avoiding any overhead
 * from unnecessary synchronisation. In the second phase, the hard-parallel portion of the reduction occurs, with
 * each work-group providing the aggregation of the point coords and counts of for each centroid based on the points
 * processed in the first phase. The host code should then aggregate these results to calculate the final adjusted
 * positions of the centroids (doing this on the GPU would require global synchronisiation).
 * 
 * The structure of the resulting output takes the following form, where in this example there are 3 centroids
 * being adjusted with the calculation split over 4 work groups:
 * 
 * |       centroid0       |       centroid1       |       centroid2       |
 * |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 * | wg0   wg1   wg2   wg3 | wg0   wg1   wg2   wg3 | wg0   wg1   wg2   wg3 |
 * 
 * See http://developer.amd.com/resources/articles-whitepapers/opencl-optimization-case-study-simple-reductions/
 * for further discussion behind GPU reduction techniques
 **/

__kernel void adjustCentroids(__global float4* points,
                            __global int* classes,
                            uint numPoints,
                            __global float4* centers,
                            uint numCenters,
                            __local float4* local_centers,
                            __local uint* local_counts,
                            __global float4* work_group_centers,
                            __global uint* work_group_counts) {

    uint g_id = get_global_id(0);
    uint l_id = get_local_id(0);

    uint g_size = get_global_size(0);
    uint l_size = get_local_size(0);

    uint num_local_centers = get_local_size(0);

    // Initialise scratch space so that each work-item has a zeroed vector and count accumulator for each centroid
    for (int center_index=0; center_index<numCenters; center_index++) {
        uint center_offset = center_index*num_local_centers;
        local_centers[center_offset + l_id] = (float4)(0,0,0,0);
        local_counts[center_offset + l_id] = 0;
    }

    // Have each work-item step through the global list of points and update local centroids in global_size strides
    while (g_id < numPoints) {
        uint current_class = classes[g_id];
        float4 current_point = points[g_id];

        // increment accumulators for the correct centroid given by the classification
        local_centers[current_class*num_local_centers + l_id] += current_point;
        local_counts[current_class*num_local_centers + l_id] += 1;

        g_id += g_size;
    }

    // Wait until all other work-items in the work-group are done before beginning cross-work-item reduction
    barrier(CLK_LOCAL_MEM_FENCE);

    // For each work-group, perform the reduction, halving the number of active work-items each step
    for (int offset = l_size/2; offset>0; offset = offset/2) {
        if (l_id < offset) {
            // Step through each centroid 
            for (int center_index=0; center_index<numCenters; center_index++) {
                uint center_offset = center_index*num_local_centers;
                // Take copies of both accumulators from the current work item and add them to another work-item's
                float4 this_vec = local_centers[center_offset + l_id];
                uint this_count = local_counts[center_offset + l_id];
                float4 other_vec = local_centers[center_offset + l_id+offset];
                uint other_count = local_counts[center_offset + l_id+offset];
                local_centers[center_offset + l_id] = this_vec + other_vec;
                local_counts[center_offset + l_id] = this_count + other_count;
            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    // Copy the reduced results of each work group from 0th position in local memory to global memory
    uint wg_id = get_group_id(0);
    uint wg_size = get_num_groups(0);
    if (l_id == 0) {
        for (int center_index=0; center_index<numCenters; center_index++) {
            work_group_centers[center_index*wg_size + wg_id] = local_centers[center_index*l_size];
            work_group_counts[center_index*wg_size + wg_id] = local_counts[center_index*l_size];
            //printf("%v4f, %lu\n", work_group_centers[center_index*wg_size + wg_id], work_group_counts[center_index*wg_size + wg_id]);        
        }
    }

    /***** Uncomment this section to update centers from within the kernel - requires global synchronisation,
     * So it makes more sense to just use the kernel exit point as the synchronisation point and let the host
     * do the final accumulation work, although that doesn't account for unnecessary data transfers introduced
     * when performing multiple iterations
    
    // Implement global spinlock here

    // Accumulate the results for each work group and update the centers
    if (get_global_id(0)==0) {
        for (int center_index=0; center_index<numCenters; center_index++) {
            float4 center_accumulator = (0,0,0,0);
            int count_accumulator = 0;
            for (int wg_index=0; wg_index < wg_size; wg_index++) {
                centers[center_index] += work_group_centers[center_index*wg_size + wg_index];
                count_accumulator += work_group_centers[center_index*wg_size + wg_index];
            }
            centers[center_index] = center_accumulator / count_accumulator;
        }
    }
    */

}