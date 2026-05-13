#ifndef OUTQUEUE_H
#define OUTQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <atomic>

#include "../../headers/Common/utils.h" 

using namespace Utils;

class OutQueue final {
public:
    void push(OutPacket::Packet packet);
    void stop();
    std::optional<OutPacket::Packet> pop();
    
private:
    std::queue<OutPacket::Packet> m_out;

    std::mutex m_mtx;
    std::condition_variable m_cv;

    std::atomic<bool> m_stop {};
};

#endif // OUTQUEUE_H