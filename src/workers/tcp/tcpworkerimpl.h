#ifndef PROJECT_TCPWORKERIMPL_H
#define PROJECT_TCPWORKERIMPL_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <functional>
#include <optional>
#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <condition_variable>

class TcpWorkerImpl : public std::enable_shared_from_this<TcpWorkerImpl> {
public:
    TcpWorkerImpl();
    ~TcpWorkerImpl();

    void Bind(const std::string& bind_address, const std::string& port);
    void Connect(const std::string& connect_address, const std::string& connect_port);
    bool Disconnect();

    using Message = std::vector<char>;

    int Send(Message message);
    int Receive(Message& message);

    void SetDelimiter(const char& delimiter);

    using CallbackFunctionType = std::function<void(const Message&)>;
    bool BindCallback(const CallbackFunctionType& callback_function);

    static std::tuple<std::vector<Message>, std::optional<Message>>
    SplitMessage(const Message& message, const std::optional<char>& delimiter);

private:
    class MessageQueue {
    public:
        enum class Error { Empty = 0, Terminated = 1 };
        // Implement Lockable interface
        void lock() { mutex_.lock(); };
        void unlock() { mutex_.unlock(); };
        bool try_lock() { return mutex_.try_lock(); }

        void terminate() { terminate_ = true; }
        bool terminated() { return terminate_.load(); }

        std::queue<Message>& get_queue() {
            return queue_;
        }

        // Implement queue interface
        bool empty() { return queue_.empty(); }
        size_t size() { return queue_.size(); }
        void push(Message&& message) { queue_.push(message); }
        void pop() { queue_.pop(); }
        void swap(std::queue<Message>& other) { queue_.swap(other); }
        Message front() const { return queue_.front(); }
        void emplace(Message message)
        {
            std::cout << "asd" << std::endl;
            queue_.emplace(message);
        }

        // Expose condition variable
        void notify_all() { condition_.notify_all(); }
        void wait(std::unique_lock<MessageQueue>& lock)
        {
            condition_.wait(lock, [this] { return terminate_.load() || !queue_.empty(); });
        };

    private:
        std::mutex mutex_;
        std::condition_variable_any condition_;
        std::queue<Message> queue_;
        std::atomic_bool terminate_{false};
    };

    std::mutex future_mutex_;

    std::future<void> process_future_;
    std::unique_ptr<boost::thread> server_thread_;

    std::shared_ptr<boost::asio::io_context> context_;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;

    std::optional<CallbackFunctionType> callback_function_;
    std::optional<char> delimiter_;

    MessageQueue queue_;

    void StartServer(const std::string& addr, const std::string& port);
    void StartAccept(boost::asio::ip::tcp::acceptor& acceptor);
    void HandleAccept(const boost::system::error_code& error,
                      std::shared_ptr<boost::asio::ip::tcp::socket> socket,
                      boost::asio::ip::tcp::acceptor& acceptor);

    void ProcessEvents();

    std::tuple<std::vector<Message>, std::optional<Message>>
    ReceiveEvents(boost::asio::ip::tcp::socket& socket);
};

#endif // PROJECT_TCPWORKERIMPL_H
