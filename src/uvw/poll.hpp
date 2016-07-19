#pragma once


#include <utility>
#include <memory>
#include <uv.h>
#include "event.hpp"
#include "handle.hpp"
#include "util.hpp"


namespace uvw {


class Poll final: public Handle<Poll> {
    static void startCallback(uv_poll_t *handle, int status, int events) {
        Poll &poll = *(static_cast<Poll*>(handle->data));
        if(status) { poll.publish(ErrorEvent{status}); }
        else { poll.publish(FlagsEvent<Poll::Event>{events}); }
    }

    explicit Poll(std::shared_ptr<Loop> ref)
        : Handle{HandleType<uv_poll_t>{}, std::move(ref)}
    { }

public:
    enum class Event {
        READABLE = UV_READABLE,
        WRITABLE = UV_WRITABLE,
        DISCONNECT = UV_DISCONNECT
    };

    template<typename... Args>
    static std::shared_ptr<Poll> create(Args&&... args) {
        return std::shared_ptr<Poll>{new Poll{std::forward<Args>(args)...}};
    }

    bool init(FileDescriptor fd) { return initialize<uv_poll_t>(&uv_poll_init, static_cast<FileDescriptor::Type>(fd)); }

    void start(Flags<Event> flags) { invoke(&uv_poll_start, get<uv_poll_t>(), flags, &startCallback); }
    void start(Event event) { start(Flags<Event>{event}); }
    void stop() { invoke(&uv_poll_stop, get<uv_poll_t>()); }
};


}
