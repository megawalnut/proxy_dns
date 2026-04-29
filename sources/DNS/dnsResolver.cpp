#include "../../headers/DNS/dnsResolver.h"

DNSResolver::DNSResolver(const char* resolvingAddress) {
    m_upstream.sin_family = AF_INET;
    m_upstream.sin_port = htons(UDP_DNS_PORT);
    if(inet_pton(AF_INET, resolvingAddress, &m_upstream.sin_addr) != 1) {
        throw std::invalid_argument("DNSResolver::DNSResolver: Failded resolving address");
    }

    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(m_socket < 0) {
        perror("DNSResolver::DNSResolver: Socket failed");
        throw std::runtime_error("Socket failed");
    }

    // for block receive if server is unreacheble
    timeval tv{};
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

bool DNSResolver::resolve(const uint8_t* packet, std::size_t size) {
    if(m_socket < 0) {
        perror("DNSResolver::resolve: Socket close");
        return false;
    }

    int senSize = sendto(m_socket, packet, size, 0, (sockaddr*)&m_upstream, sizeof(m_upstream));
    if(senSize <= 0) {
        perror("DNSResolver::resolve: Failed sendto");
        return false;
    }

    sockaddr_in from{};
    socklen_t lenFrom = sizeof(from);

    ssize_t recSize = recvfrom(m_socket, m_buffer, BUFFER_SIZE, 0, (sockaddr*)&from, &lenFrom);
    if(recSize <= 0) {
        perror("DNSResolver::resolve: Failed recvfrom");
        return false;
    }

    if(from.sin_addr.s_addr != m_upstream.sin_addr.s_addr ||
        from.sin_port != m_upstream.sin_port) {
        perror("DNSResolver::resolve: Unknown sender");
        return false;
    }

    m_receiveSize = recSize;

    return true;
}

const uint8_t* DNSResolver::getReceive() const {
    return m_buffer;
}

std::size_t DNSResolver::getReceiveSize() const {
    return m_receiveSize;
}

DNSResolver::~DNSResolver() {
    if(m_socket >= 0) {
        close(m_socket);
    }
}