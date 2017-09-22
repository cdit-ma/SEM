__kernel void classifyPoints(__global float4* points,
                                __global uint* classes,
                                uint numPoints,
                                __global float4* centers,
                                uint numCenters) {
    uint index = get_global_id(0)%numPoints;
    float minDist = FLT_MAX;
    float currentDist;
    
    for (uint i=0; i<numCenters; i++) {
        currentDist = distance(points[index], centers[i%4]);
        if (currentDist < minDist) {
            minDist = currentDist;
            classes[index] = i%4;
        }
    }
}


__kernel void adjustCentroids(__global float4* points,
                                __global uint* classes,
                                uint numPoints,
                                __global float4* centers,
                                __global uint* centerPointCount,
                                uint numCenters,
                                __local float4* localCenters,
                                __local uint* localCounts,
                                __global float4* centerAggregator,
                                __global uint* centerCountAggregator) {
        
    uint localID = get_local_id(0);
    uint pointIndex = get_global_id(0);
    uint groupID = get_group_id(0);

    uint elementsPerCenter = get_local_size(0)*2;
    uint totalElements = numCenters*elementsPerCenter;

    for (uint i=0; i<numCenters; i++) {
        localCenters[localID + i*elementsPerCenter] = 0.0;
        localCenters[localID + get_local_size(0) + i*elementsPerCenter] = 0.0;
        localCounts[localID + i*elementsPerCenter] = 0;
        localCounts[localID + get_local_size(0) + i*elementsPerCenter] = 0;
    }
    
    if  (pointIndex*2<numPoints) {
        uint curClass = classes[pointIndex];
        localCenters[localID + curClass*elementsPerCenter] = points[pointIndex];
        localCounts[localID + curClass*elementsPerCenter] = 1;
        curClass = classes[pointIndex+numPoints/2];
        localCenters[localID + curClass*elementsPerCenter] +=  points[pointIndex+numPoints/2];
        localCounts[localID + curClass*elementsPerCenter] += 1;
    } 
        
    for (uint i=get_local_size(0); i>0; i>>=1) {
        if (localID<i) {
            for (uint centerOffset=0; centerOffset<totalElements; centerOffset+=elementsPerCenter) {
                localCenters[centerOffset+localID] += localCenters[centerOffset+localID+i];
                localCounts[centerOffset+localID] += localCounts[centerOffset+localID+i];
            }
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (localID<numCenters) {
        centerAggregator[groupID*numCenters+localID] = localCenters[localID*elementsPerCenter];
        centerCountAggregator[groupID*numCenters+localID] = localCounts[localID*elementsPerCenter];
    }

    barrier(CLK_GLOBAL_MEM_FENCE);

    if (pointIndex<numCenters) {
        centers[pointIndex] = 0.0;
        centerPointCount[pointIndex] = 0;
        uint numGroups = get_global_size(0)/get_local_size(0);
        for (uint i=0; i<numGroups; i++) {
            centers[pointIndex] += centerAggregator[i*numCenters+pointIndex];
            centerPointCount[pointIndex] += centerCountAggregator[i*numCenters+pointIndex];
        }
        centers[pointIndex] /= centerPointCount[pointIndex];
    }
    

}