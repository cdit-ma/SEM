//
// Created by Jackson Michael on 10/12/20.
//

#ifndef SEM_FFT_ACCEL_EVENT_DISPATCHER_HPP
#define SEM_FFT_ACCEL_EVENT_DISPATCHER_HPP

#include "data/fft_data.hpp"

#include "result.hpp"

#include <vector>
#include <future>

namespace sem::fft_accel::runtime {

    /**
     * Provides an interface for dispatching results on a different thread to the one on which they were produced.
     * Events will by default be dispatched to a call
     * @tparam EventID
     * @tparam EventData
     */
    template<typename EventID, typename EventData>
    class event_dispatcher {
    public:
        using CallbackSignature = void (EventID, EventData);
        using Callback = std::function<CallbackSignature>;

        virtual ~event_dispatcher() = 0;

        virtual sem::Result<void> submit_event(EventID id, EventData data) = 0;
        virtual sem::Result<void> register_callback(Callback callback) = 0;
        virtual sem::Result<std::future<EventData>> request_event(EventID) = 0;
    };

    template<typename EventID, typename EventData>
    inline event_dispatcher<EventID, EventData>::~event_dispatcher<EventID, EventData>() = default;
}


#endif //SEM_FFT_ACCEL_EVENT_DISPATCHER_HPP
