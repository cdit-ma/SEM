#ifndef RE_OPENCL_QUEUE_H
#define RE_OPENCL_QUEUE_H

#include <memory>

namespace cl {
class CommandQueue;
}

namespace Re {

namespace OpenCL {

class Manager;
class Device;

class Queue {
    public:
    Queue(const Manager& manager, const Device& device);

    cl::CommandQueue& GetRef() const;

    private:
    std::unique_ptr<cl::CommandQueue> queue_;
    const Manager& manager_;
    const Device& dev_;
};

} // namespace OpenCL

} // namespace Re

#endif // RE_OPENCL_QUEUE_H