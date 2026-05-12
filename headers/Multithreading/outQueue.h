#ifndef OUTQUEUE_H
#define OUTQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <atomic>

#include "../DNS/packet.h"

class OutQueue final {
public:
    ~OutQueue();
    
    void push(Packet packet);
    std::optional<Packet> pop();
    
private:
    std::queue<Packet> m_out;

    std::mutex m_mtx;
    std::condition_variable m_cv;

    std::atomic<bool> m_stop {};
};

#endif // OUTQUEUE_H