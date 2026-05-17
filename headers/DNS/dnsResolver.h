#ifndef DNSRESOLVER_H
#define DNSRESOLVER_H

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mutex>
#include <numeric>
#include <atomic>

#include "dnsParser.h"
#include "../Common/utils.h"

using namespace Utils;

class DNSResolver final {
    static constexpr inline std::size_t BUFFER_SIZE = 512;
    static constexpr inline int UDP_DNS_PORT = 53;
    static constexpr inline int RESOLVE_SIZE = 1000;

public:
    struct Packet {
        Parse::Status status = Parse::Status::Err;
        DNSParser::DNSPtr packet = DNSParser::DNSPtr(nullptr);
        std::string error = "";
    };

    DNSResolver(const std::string& addr);

    DNSResolver::Packet resolve(const DNSParser::DNSPtr& packet);
    double getResolve();
    void resetLatency();

private:
    static int makeSocket();
    void addResolve(std::chrono::nanoseconds duration);

private:
    sockaddr_in m_upstream;
    std::mutex m_mtxRes;
    
    std::atomic<uint32_t> m_index {};
    std::vector<std::chrono::nanoseconds> m_resolve;
};

#endif // DNSRESOLVER_H