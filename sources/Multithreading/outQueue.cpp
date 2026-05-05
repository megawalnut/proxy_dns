#include "../../headers/Multithreading/outQueue.h"

void OutQueue::push(Packet packet) {
    {
        std::lock_guard lock{ m_mtx };
        m_out.push(std::move(packet));
    }
    m_cv.notify_one();
}

std::optional<Packet> OutQueue::pop() {
    std::unique_lock lock{ m_mtx };
    m_cv.wait(lock, [this]() {
        return m_stop || !m_out.empty();
    });
        
    if (m_stop && m_out.empty()) {
        return std::nullopt;
    }

    auto receive = std::move(m_out.front());
    m_out.pop();

    return receive;
}

OutQueue::~OutQueue() {
    {
        std::lock_guard lock{ m_mtx };
        m_stop = true;
    }
    m_cv.notify_all();
}