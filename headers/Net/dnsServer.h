#ifndef DNSSERVER_H
#define DNSSERVER_H

#include <iostream>
#include <sys/socket.h>
#include <atomic> 

#include "../DNS/dnsParser.h"
#include "../DNS/dnsDispatcher.h"
#include "../DNS/packet.h"
#include "../Multithreading/threadPool.h"
#include "../Multithreading/Tasks/task.h"
#include "../Multithreading/Tasks/dnsTask.h"
#include "../Common/utils.h"

class DNSServer final {
    // we specifically limit the buffer, since the proxy does not support TC
    static constexpr inline std::size_t BUFFER_SIZE = 512;
    static constexpr inline int UDP_DNS_PORT = 53;
    static constexpr inline int QUEUE_SIZE = 10;

public:
    DNSServer(DNSDispatcher& disp);
    ~DNSServer();

    bool run();

    bool isRunning() const;
    double getStartTime() const;
    uint64_t getTotalRequests() const;
    double getLatency();
    double getErrors() const;
    
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