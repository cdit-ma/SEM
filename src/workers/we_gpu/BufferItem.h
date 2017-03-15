// WE_GPU.mpc  2015-08-17  Jackson Michael 

#ifndef BUFFERITEM_H
#define BUFFERITEM_H

#include "cl.hpp"


// Convenient linked list item for storing an OpenCL buffer
class BufferItem {
public:

	BufferItem* next;

	size_t size;
	cl::Buffer* buffer;
};

#endif