//
// Created by Jackson Michael on 18/3/21.
//

#ifndef SEM_TCP_STREAM_LISTENER_HPP
#define SEM_TCP_STREAM_LISTENER_HPP

namespace sem::logging::netdata {

    class tcp_stream_listener {
    public:
        virtual ~tcp_stream_listener() = 0;

        virtual void receive_line(std::string tcp_stream_line) = 0;
    };

    inline tcp_stream_listener::~tcp_stream_listener() = default;
}

#endif //SEM_TCP_STREAM_LISTENER_HPP
