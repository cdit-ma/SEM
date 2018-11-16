#ifndef OPENCLQUEUE_H
#define OPENCLQUEUE_H

#include <memory>
class OpenCLManager;
class OpenCLDevice;

namespace cl {
    class CommandQueue;
};

class OpenCLQueue {
public:
    OpenCLQueue(const OpenCLManager& manager, const OpenCLDevice& device);
    ~OpenCLQueue() = default;

    cl::CommandQueue& GetRef() const;

private:
    std::shared_ptr<cl::CommandQueue> queue_;
    const OpenCLManager& manager_;
    const OpenCLDevice& dev_;
};

#endif //OPENCLQUEUE_H