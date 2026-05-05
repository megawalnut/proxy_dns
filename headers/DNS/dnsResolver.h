#ifndef DNSRESOLVER_H
#define DNSRESOLVER_H

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mutex>

#include "dnsParser.h"
#include "../Common/utils.h"

class DNSResolver final {
    static constexpr std::size_t BUFFER_SIZE = 512;
    static constexpr int UDP_DNS_PORT = 53;
public: 
    DNSResolver(const std::string& addr);

    DNSParser::DNSPkt resolve(const DNSParser::DNSPtr& packet);

private:
    static int makeSocket();

private:
    sockaddr_in m_upstream;
    std::mutex m_mtx;
};

#endif // DNSRESOLVER_H