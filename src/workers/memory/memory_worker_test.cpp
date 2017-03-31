#include <iostream>
#include "memory_worker_impl.h"

int main(){
    std::cout << "Memory worker tester" << std::endl;
    
    std::string in;
    Memory_Worker_Impl* worker = new Memory_Worker_Impl();

    std::cout << "ALLOCATING" << std::endl;
    worker->Allocate(1024*1024);
    std::cout << "ALLOCATED" << std::endl;
    std::cout << worker->GetAllocatedCount() << std::endl;
    std::cin >> in;

    std::cout << "DEALLOCATING" << std::endl;
    worker->Deallocate(1024*1024);
    std::cout << "DEALLOCATED" << std::endl;
    std::cout << worker->GetAllocatedCount() << std::endl;
    std::cin >> in;
    
    std::cout << "ALLOCATING" << std::endl;
    worker->Allocate(1024*1024/2);
    std::cout << "ALLOCATED" << std::endl;
    std::cout << worker->GetAllocatedCount() << std::endl;
    std::cin >> in;

    delete worker;

    return 0;
}