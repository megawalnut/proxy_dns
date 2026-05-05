#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "outQueue.h"
#include "Tasks/task.h"

static constexpr inline uint32_t WORKERS_SIZE = 6;

class ThreadPool {
public:
    ThreadPool(std::size_t workers = WORKERS_SIZE);
    ~ThreadPool();

    void enqueue(std::shared_ptr<Task> newTask);
    std::optional<Packet> popResult();
    
private:
    void workerLoop();

private:
    std::vector<std::thread> m_workers;
    std::queue<std::shared_ptr<Task>> m_tasks;

    std::mutex m_mtx;
    std::condition_variable m_cv;

    OutQueue m_out;
    std::atomic<bool> m_stop = false;
};

#endif // THREADPOOL_H