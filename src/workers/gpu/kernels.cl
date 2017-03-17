CL_KERNEL_CODE(
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
			}

			barrier(CLK_LOCAL_MEM_FENCE);
		}
		
		if (eRow < M && eCol < N) {
			out[eCol + eRow*N] = eSum;
		}
	}
)