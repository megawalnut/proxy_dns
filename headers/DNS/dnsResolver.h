#ifndef DNSRESOLVER_H
#define DNSRESOLVER_H

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "dnsParser.h"
#include "../utils.h"

class DNSResolver final {
    static constexpr std::size_t BUFFER_SIZE = 512;
    static constexpr int UDP_DNS_PORT = 53;
public: 
    DNSResolver(const std::string& addr);
    ~DNSResolver();

    DNSParser::DNSPkt resolve(const DNSParser::DNSPtr& packet);

private:
    int m_socket = -1;
    sockaddr_in m_upstream;
};

#endif // DNSRESOLVER_H