#include "../../headers/DNS/dnsResolver.h"

DNSResolver::DNSResolver(const std::string& addr/*8.8.8.8*/) {
    m_upstream.sin_family = AF_INET;
    m_upstream.sin_port = htons(UDP_DNS_PORT);
    if(inet_pton(AF_INET, addr.c_str(), &m_upstream.sin_addr) != 1) {
        throw std::invalid_argument("Failed resolving address");
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

    if(setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("DNSResolver::DNSResolver: Setsockopt failed");
        close(m_socket);
        throw std::runtime_error("Setsockopt failed");
    }
}

DNSParser::DNSPkt DNSResolver::resolve(const DNSParser::DNSPtr& packet) {
    if(m_socket < 0) {
        perror("DNSResolver::resolve: Socket closed");
        return { Utils::Parse::Status::Err, nullptr }; 
    }

    // sserialize ldns packet to wire bytes 
    const std::vector<uint8_t> question = DNSParser::serialize(packet);

    // send to remote host
    ssize_t senSize = sendto(m_socket, question.data(), question.size(), 0, 
            reinterpret_cast<sockaddr*>(&m_upstream), sizeof(m_upstream));

    if(senSize <= 0) {
        perror("DNSResolver::resolve: Failed sendto");
        return { Utils::Parse::Status::Err, nullptr }; 
    }

    sockaddr_in from{};
    socklen_t len = sizeof(from);

    // buffer for aPacket
    std::vector<uint8_t> answer;
    answer.resize(BUFFER_SIZE);

    // get receive
    ssize_t recSize = recvfrom(m_socket, answer.data(), answer.size(),
             0, reinterpret_cast<sockaddr*>(&from), &len);

    if(recSize <= 0) {
        perror("DNSResolver::resolve: Failed recvfrom");
        return { Utils::Parse::Status::Err, nullptr }; 
    }

    // validate host
    if(from.sin_addr.s_addr != m_upstream.sin_addr.s_addr ||
        from.sin_port != m_upstream.sin_port) {
        std::cerr << "DNSResolver::resolve: Unknown sender" << std::endl;
        return { Utils::Parse::Status::Err, nullptr }; 
    }

    // delete unused data
    answer.resize(recSize);

    return DNSParser::deserialize(answer);
}

DNSResolver::~DNSResolver() {
    if(m_socket >= 0) {
        close(m_socket);
    }
}