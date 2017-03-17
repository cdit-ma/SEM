// WE_GPU.mpc  2015-08-17  Jackson Michael 

#ifndef BUFFERHASHTABLE_H
#define BUFFERHASHTABLE_H

#include "cl.hpp"
#include "BufferItem.h"

typedef unsigned char hash_index_t;

// Chained hash table for allocating and deallocating OpenCL buffers, hiding all information
// so that buffers need only be addressed by size.
class BufferHashtable {

public:
	// Creates a hastable for storing OpenCL buffers indexed by size, only valid so long as the supplied context is valid
	BufferHashtable(cl::Context* context);
	~BufferHashtable();

	// Attempts to allocate memory of a given size, returning the associated buffer if successful
	// or null if not
	cl::Buffer* allocate(size_t bytes);

	// Looks for previously allocated buffer of a given size and deletes it if one is found, returning
	// whether or not a deallocation occured
	bool deallocate(size_t bytes);

	// Retrieve a buffer of a given size, useful for copy busy-work. Returns NULL if no matching buffer is found.
	cl::Buffer* get(size_t bytes);

	// Returns the amount of graphics memor as seen by OpenCL, may not all be available
	size_t getMemTotal();

	// Returns the amount of graphics memory used to allocate the current buffers, can't know about
	// how much memory other classes or processes are using.
	size_t getMemUsed();

private:
	// Amount of possible hash values determines the size of the hashtable
	static const unsigned long TABLE_SLOTS = (1 << (sizeof(hash_index_t)*8));
	BufferItem* table[TABLE_SLOTS];

	cl::Context* context_;

	size_t memUsed;

	hash_index_t hash(size_t byteAmount);

	//ACE_Thread_Mutex lock_;
};

#endif