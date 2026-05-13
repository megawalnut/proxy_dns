#include "../../headers/Multithreading/threadPool.h"

ThreadPool::ThreadPool(std::size_t workers) {
    m_workers.reserve(workers);
    m_latency.reserve(LATENCY_SIZE);

    for(std::size_t i = 0; i < workers; ++i) {
        m_workers.emplace_back([this]() {
            workerLoop(); 
        });
    }
}

void ThreadPool::addLatency(std::chrono::nanoseconds duration) {
    std::lock_guard lock{ m_mtxLat };
    if(m_index < LATENCY_SIZE)
        m_latency.push_back(duration);
    else
        m_latency[m_index % LATENCY_SIZE] = duration;   // push_front
    ++m_index;
}

double ThreadPool::getLatency() {
    std::vector<std::chrono::nanoseconds> copy;
    {
        std::lock_guard lock{m_mtxLat};
        copy = m_latency;
        if (m_index >= LATENCY_SIZE) {
            m_latency.clear();
            m_index = 0;
        }
    }
    std::sort(copy.begin(), copy.end());

    if(copy.empty()) {
        return {};
    }

    std::size_t count = copy.size() * 0.95;
    if(count >= copy.size()) {
        --count;
    }

    return std::chrono::duration<double, std::milli>(copy[count]).count();  // 95 percentile
}

double ThreadPool::getTotalRequests() const {
    return m_totalRequests;
}

double ThreadPool::getErrors() const {
    if(m_totalRequests == 0) {
        return {};
    }
    return static_cast<double>(m_totalErrors) / 
           static_cast<double>(m_totalRequests) * 100.0;
}

void ThreadPool::workerLoop() {
    while(true) {
        std::shared_ptr<Task> current;

        {
            std::unique_lock lock{ m_mtx };
            
            m_cv.wait(lock, [this](){
                return m_stop || !m_tasks.empty();
            });

            if(m_stop && m_tasks.empty()) {
                return;
            }

            current = std::move(m_tasks.front());
            m_tasks.pop();
        }

        ++m_totalRequests;
        bool isOk;

        auto start = std::chrono::steady_clock::now();
        current->execute(m_out,isOk);
        addLatency(std::chrono::steady_clock::now() - start);
        if(!isOk) {
            ++m_totalErrors;
        }
    }
}

void ThreadPool::enqueue(std::shared_ptr<Task> task) {
    {
        std::lock_guard lock{ m_mtx };
        if(m_stop) {
            return;
        }

        m_tasks.push(std::move(task));
    }

    m_cv.notify_one();
}

std::optional<OutPacket::Packet> ThreadPool::popResult() {
    return m_out.pop();
}

void ThreadPool::stopQueue() {
    m_out.stop();
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard lock{ m_mtx };
        m_stop = true;
    }

    m_cv.notify_all();

    for(auto& w : m_workers) {
        if(w.joinable()) {
            w.join();
        }
    }
}