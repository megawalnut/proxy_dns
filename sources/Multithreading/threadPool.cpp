#include "../../headers/Multithreading/threadPool.h"

ThreadPool::ThreadPool(std::size_t workers) {
    m_workers.reserve(workers);

    for(std::size_t i = 0; i < workers; ++i) {
        m_workers.emplace_back([this]() {
            workerLoop(); 
        });
    }
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

        current->execute(m_out);
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

std::optional<Packet> ThreadPool::popResult() {
    return m_out.pop();
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