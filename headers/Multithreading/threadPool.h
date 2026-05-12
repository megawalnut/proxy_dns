#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>


#include "outQueue.h"
#include "Tasks/task.h"

class ThreadPool {
    static constexpr inline uint32_t WORKERS_SIZE = 6;
    static constexpr inline uint32_t LATENCY_SIZE = 1000;
public:
    ThreadPool(std::size_t workers = WORKERS_SIZE);
    ~ThreadPool();

    void enqueue(std::shared_ptr<Task> newTask);
    std::optional<Packet> popResult();
    double getLatency();
    double getTotalRequests() const;
    double getErrors() const;

private:
    void workerLoop();
    void addLatency(std::chrono::nanoseconds duration);

private:
    std::vector<std::thread> m_workers;
    std::queue<std::shared_ptr<Task>> m_tasks;

    std::mutex m_mtx;
    std::mutex m_mtxLat;
    std::condition_variable m_cv;

    OutQueue m_out;
    std::atomic<bool> m_stop {};

    std::vector<std::chrono::nanoseconds> m_latency;
    std::atomic<uint32_t> m_index{};
    std::atomic<uint64_t> m_totalRequests{};
    std::atomic<uint64_t> m_totalErrors{};
};

#endif // THREADPOOL_H