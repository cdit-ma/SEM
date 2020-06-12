
#include <memory>


#include "tcpworkerimpl.h"
#include <boost/bind.hpp>
#include <iostream>

// Boost deprecated the old get_io_service() in favour of get_executor() since 1.67
#if BOOST_VERSION >= 107000
#define GET_IO_SERVICE(s) ((boost::asio::io_context&)(s).get_executor().context())
#else
#define GET_IO_SERVICE(s) ((s).get_io_service())
#endif

TcpWorkerImpl::TcpWorkerImpl() { context_ = std::make_shared<boost::asio::io_context>(); }

TcpWorkerImpl::~TcpWorkerImpl() {}

// Connects a TCP sender/receiver to the attribute address and port
void TcpWorkerImpl::Connect(const std::string& connect_address, const std::string& port)
{
    using boost::asio::ip::tcp;
    socket_ = std::make_shared<tcp::socket>(*context_);
    try {
        auto addr = boost::asio::ip::address::from_string(connect_address);
        auto port_int = std::stoi(port);

        tcp::endpoint ep(addr, port_int);
        socket_->connect(ep);
    } catch(const std::exception& ex) {
        socket_->close();
        throw std::logic_error("Tried to connect to invalid endpoint");
    }
}

// Send a message of data to the endpoint we've connected to
int TcpWorkerImpl::Send(Message data)
{
    if(!socket_) {
        throw std::logic_error("Tried to send with no port.");
    }
    return (int)boost::asio::write(*socket_, boost::asio::buffer(data, data.size()));
}

bool TcpWorkerImpl::Disconnect()
{
    queue_.terminate();
    queue_.notify_all();
    if(socket_) {
        socket_->close();
    }

    context_->stop();
    if(server_thread_) {
        server_thread_->join();
    }

    try {
        if(process_future_.valid()) {
            process_future_.get();
        }
    } catch(const std::exception& ex) {
        throw;
    }
    return true;
}

// Binds a receiving tcp socket to the attribute address and port
void TcpWorkerImpl::Bind(const std::string& bind_address, const std::string& port)
{
    std::lock_guard<std::mutex> lock(future_mutex_);

    // Fail if we have process or receive threads already running
    if(process_future_.valid() || server_thread_) {
        throw std::logic_error("Tried to re-bind an already bound tcp worker.");
    }

    // Start process thread if we have a valid callback function
    if(callback_function_) {
        process_future_ = std::async(std::launch::async, &TcpWorkerImpl::ProcessEvents,
                                     shared_from_this());
    }

    // Start receive thread even if we dont have a callback function
    server_thread_ = std::make_unique<boost::thread>(&TcpWorkerImpl::StartServer,
                                                     shared_from_this(), bind_address, port);
}

bool TcpWorkerImpl::BindCallback(const CallbackFunctionType& callback_function)
{
    callback_function_ = callback_function;
    return true;
}

void TcpWorkerImpl::StartServer(const std::string& addr, const std::string& port)
{
    using boost::asio::ip::tcp;
    auto address = boost::asio::ip::address::from_string(addr);
    auto port_int = std::stoi(port);
    tcp::acceptor acceptor(*context_, tcp::endpoint(address, port_int));

    // Add a job to start accepting connections.
    StartAccept(acceptor);

    // Process event loop.
    context_->run();
}

void TcpWorkerImpl::HandleAccept(const boost::system::error_code& error,
                                 std::shared_ptr<boost::asio::ip::tcp::socket> socket,
                                 boost::asio::ip::tcp::acceptor& acceptor)
{
    // If there was an error, then do not add any more jobs to the service.
    if(error) {
        return;
    }

    socket_ = socket;
    // Store partial messages when we hit a delimiter
    std::optional<Message> partial_message{};
    while(true) {
        try {
            auto [messages, partial_message] = ReceiveEvents(*socket);
            std::lock_guard<MessageQueue> lock(queue_);
            for(Message& message : messages) {
                if(partial_message) {
                    // If we have a partial message, prepend it to the front of the first
                    // message before pushing to process queue.
                    partial_message->insert(partial_message->end(), message.begin(), message.end());
                    queue_.get_queue().emplace(partial_message.value());
                    partial_message = std::nullopt;
                } else {

                    queue_.get_queue().emplace(message);
                }
            }
            queue_.notify_all();
        } catch(const boost::system::system_error& ex) {
            break;
        }
    }
    socket_ = nullptr;

    StartAccept(acceptor);
}

void TcpWorkerImpl::StartAccept(boost::asio::ip::tcp::acceptor& acceptor)
{
    using boost::asio::ip::tcp;

    //auto socket = std::make_shared<tcp::socket>(GET_IO_SERVICE(acceptor));

    auto socket = std::make_shared<tcp::socket>(*context_);

    // Add an accept call to the service.  This will prevent io_service::run()
    // from returning.
    acceptor.async_accept(*socket, boost::bind(&TcpWorkerImpl::HandleAccept, shared_from_this(),
                                               boost::asio::placeholders::error, socket,
                                               boost::ref(acceptor)));
}

auto TcpWorkerImpl::ReceiveEvents(boost::asio::ip::tcp::socket& socket)
    -> std::tuple<std::vector<TcpWorkerImpl::Message>, std::optional<TcpWorkerImpl::Message>>
{
    std::vector<Message> split_messages{};
    char data[1024];
    try {
        auto count = socket.read_some(boost::asio::buffer(data));

        Message data_vec{};
        data_vec.assign(data, data + count);
        return SplitMessage(data_vec, delimiter_);

    } catch(const boost::system::system_error& ex) {
        throw;
    }
}

// Blocking receive call
int TcpWorkerImpl::Receive(Message& out)
{
    // Get entire message queue
    std::unique_lock<MessageQueue> lock(queue_);
    while(true) {
        if(queue_.terminated()) {
            return (int)MessageQueue::Error::Terminated;
        }
        if(queue_.empty()) {
            return (int)MessageQueue::Error::Empty;
        }
        Message message = queue_.get_queue().front();
        for(const auto& byte : message) {
            out.push_back(byte);
        }
        queue_.get_queue().pop();
    }
}

void TcpWorkerImpl::ProcessEvents()
{
    std::queue<Message> swap_queue{};
    while(true) {
        {
            std::unique_lock<MessageQueue> lock(queue_);
            queue_.wait(lock);
            queue_.get_queue().swap(swap_queue);

            if(queue_.terminated()) {
                break;
            }
        }

        while(!swap_queue.empty()) {
            auto message = std::move(swap_queue.front());
            swap_queue.pop();
            if(callback_function_) {
                try {
                    callback_function_.value()(message);
                } catch(const std::exception& ex) {
                }
            }
        }
    }
}

auto TcpWorkerImpl::SplitMessage(const TcpWorkerImpl::Message& message,
                                 const std::optional<char>& delimiter)
    -> std::tuple<std::vector<TcpWorkerImpl::Message>, std::optional<TcpWorkerImpl::Message>>
{
    if(!delimiter) {
        // If we don't have a delimiter, give back the message and a std::nullopt
        return {{message}, std::nullopt};
    }

    std::vector<Message> messages{};
    Message partial_message{};
    Message current{};

    for(const auto& element : message) {
        if(element == delimiter) {
            messages.emplace_back(current);
            current.clear();
        } else {
            current.emplace_back(element);
        }
    }

    if(!current.empty()) {
        partial_message = current;
    }

    return {messages, partial_message};
}
void TcpWorkerImpl::SetDelimiter(const char& delimiter) { delimiter_ = delimiter; }
