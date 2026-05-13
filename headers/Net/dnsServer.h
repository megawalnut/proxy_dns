#ifndef DNSSERVER_H
#define DNSSERVER_H

#include <iostream>
#include <sys/socket.h>
#include <atomic> 

#include "../DNS/dnsParser.h"
#include "../DNS/dnsDispatcher.h"
#include "../Multithreading/threadPool.h"
#include "../Multithreading/Tasks/task.h"
#include "../Multithreading/Tasks/dnsTask.h"
#include "../Common/utils.h"

using namespace Utils;

class DNSServer final {
    // we specifically limit the buffer, since the proxy does not support TC
    static constexpr inline std::size_t BUFFER_SIZE = 512;
    static constexpr inline int UDP_DNS_PORT = 53;

public:
    DNSServer(DNSDispatcher& disp);
    ~DNSServer();

    bool run();

    bool isRunning() const;
    double getStartTime() const;
    uint64_t getTotalRequests() const;
    uint64_t getTotalErrors() const;
    double getLatency();
    
private:
    void senderLoop();

private:
    int m_socket = -1;
    sockaddr_in m_serv;
    DNSDispatcher& m_dispatcher;

    ThreadPool m_threadPool;
    std::thread m_sender;
    std::atomic<bool> m_stop {};
    std::chrono::steady_clock::time_point m_startTime;
};

#endif // DNSSERVER_H