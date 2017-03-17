// WE_GPU.mpc  2015-08-17  Jackson Michael 

#include "BufferHashtable.h"
#include "BufferItem.h"
#include "clErrorPrinter.h"
#include <iostream>
#include <vector>



using namespace std;

BufferHashtable::BufferHashtable(cl::Context* context) {
	context_ = context;
	memUsed = 0;

	for (unsigned long i=0; i<TABLE_SLOTS; i++) {
		table[i] = NULL;
	}
}

BufferHashtable::~BufferHashtable() {
	//lock_.acquire();

	if (memUsed > 0) {
		cerr << "Warning: destroying buffer hashtable before all GPU memory deallocated!" << endl;
	}

	// Delete every item in the chain for each index, freeing the cl:Buffer as well
	for (unsigned long index=0; index<TABLE_SLOTS; index++) {
		BufferItem* curItem = table[index];
		while (curItem != NULL) {
			delete curItem->buffer;
			BufferItem* deadItem = curItem;
			curItem = curItem->next;
			delete deadItem;
		}
	}
	//lock_.release();
}

cl::Buffer* BufferHashtable::allocate(size_t bytes) {
	//lock_.acquire();

	hash_index_t index = hash(bytes);

	BufferItem* newBuffer = new BufferItem();

	cl_int error;
	newBuffer->buffer = new cl::Buffer(*context_, CL_MEM_ALLOC_HOST_PTR, bytes, NULL, &error);
	if (error != CL_SUCCESS) {
		cerr << "Unable to allocate GPU buffer (" << bytes << " bytes), error: "<< clGetErrorName(error) << endl;
		delete newBuffer->buffer;
		delete newBuffer;
		//lock_.release();
		return NULL;
	}

	newBuffer->size = bytes;

	newBuffer->next = table[index];
	table[index] = newBuffer;

	memUsed += bytes;

	//lock_.release();
	return newBuffer->buffer;
}

bool BufferHashtable::deallocate(size_t bytes) {
	//lock_.acquire();

	hash_index_t index = hash(bytes);

	BufferItem* curItem = table[index];
	BufferItem* prevItem = curItem;

	while (curItem != NULL) {
		if (curItem->size == bytes) {
			delete curItem->buffer;
			if (curItem == prevItem) {
				table[index] = curItem->next;
			} else {
				prevItem->next = curItem->next;
			}
			memUsed -= curItem->size;
			delete curItem;
			//lock_.release();
			return true;
		}

		prevItem = curItem;
		curItem = curItem->next;
	}

	cerr << "Couldn't deallocate a buffer of size " << bytes << " bytes; couldn't find one" << endl;
	//lock_.release();
	return false;
}

cl::Buffer* BufferHashtable::get(size_t bytes) {
	//lock_.acquire();

	hash_index_t index = hash(bytes);

	BufferItem* curItem = table[index];

	while (curItem != NULL) {
		if (curItem->size == bytes) {
			//lock_.release();
			return curItem->buffer;
		}

		curItem = curItem->next;
	}

	cerr << "Couldn't find a buffer of size " << bytes << " bytes" << endl;
	//lock_.release();
	return NULL;
}

size_t BufferHashtable::getMemTotal() {
	vector<cl::Device> devices;
	cl_ulong mem;
	size_t totalMem=0;

	context_->getInfo(CL_CONTEXT_DEVICES, &devices);

	for (unsigned int i=0; i<devices.size(); i++) {
		devices[i].getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &mem);
		totalMem += (size_t)mem;
	}

	return totalMem;
}

size_t BufferHashtable::getMemUsed() {
	return memUsed;
}


const int HASH_SHIFT_MAX = (sizeof(size_t)-1)*8;

// Adds each byte to each other byte by repeatedly shifting
hash_index_t BufferHashtable::hash(size_t bytes) {
	size_t aggregator = bytes;

	for (unsigned int shiftAmt=0; shiftAmt<HASH_SHIFT_MAX; shiftAmt+=8) {
		aggregator += (bytes << shiftAmt);
	}

	for (unsigned int shiftAmt=0; shiftAmt<HASH_SHIFT_MAX; shiftAmt+=8) {
		aggregator += (bytes >> shiftAmt);
	}

	hash_index_t result = aggregator & 0xFF;
	return result;
}