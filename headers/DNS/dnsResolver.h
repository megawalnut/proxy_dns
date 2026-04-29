#ifndef DNSRESOLVE_H
#define DNSRESOLVE_H

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern int UDP_DNS_PORT;

class DNSResolver final {
    static constexpr std::size_t BUFFER_SIZE = 2048;
public: 
    DNSResolver(const char* resolvingAddress);
    ~DNSResolver();

    bool resolve(const uint8_t* packet, std::size_t size);
    const uint8_t* getReceive() const;
    std::size_t DNSResolver::getReceiveSize() const;

private:
    sockaddr_in m_upstream;
    int m_socket = -1;
    uint8_t m_buffer[BUFFER_SIZE];
    std::size_t m_receiveSize = 0;
};

#endif // DNSRESOLVE_H