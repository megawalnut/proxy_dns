#include "../../headers/DNS/dnsResolver.h"

DNSResolver::DNSResolver(const std::string& addr/*8.8.8.8*/) {
    m_upstream.sin_family = AF_INET;
    m_upstream.sin_port = htons(UDP_DNS_PORT);
    if(inet_pton(AF_INET, addr.c_str(), &m_upstream.sin_addr) != 1) {
        throw std::invalid_argument("Failed resolving address");
    }
}

DNSParser::DNSPkt DNSResolver::resolve(const DNSParser::DNSPtr& packet) {
    thread_local int sock = makeSocket();
    if(sock < 0) {
        perror("DNSResolver::resolve: Socket closed");
        return { Utils::Parse::Status::Err, DNSParser::DNSPtr(nullptr) };
    }

    // remember for get exactly my packet
    const uint16_t id = ldns_pkt_id(packet.get());

    // sserialize ldns packet to wire bytes 
    const auto& [ok, question] = DNSParser::serialize(packet);
    if(ok != Utils::Parse::Status::Ok) {
        return { Utils::Parse::Status::Err, DNSParser::DNSPtr(nullptr) };
    }

    // send to remote host
    ssize_t senSize = sendto(sock, question.data(), question.size(), 0, 
            reinterpret_cast<sockaddr*>(&m_upstream), sizeof(m_upstream));

    if(senSize <= 0) {
        perror("DNSResolver::resolve: Failed sendto");
        return { Utils::Parse::Status::Err, DNSParser::DNSPtr(nullptr) }; 
    }

    sockaddr_in from{};
    socklen_t len = sizeof(from);

    // buffer for aPacket
    std::vector<uint8_t> answer(BUFFER_SIZE);

    while(true) {
        answer.assign(BUFFER_SIZE, 0);

        // get receive
        ssize_t recSize = recvfrom(sock, answer.data(), answer.size(),
                0, reinterpret_cast<sockaddr*>(&from), &len);

        if(recSize <= 0) {
            perror("DNSResolver::resolve: Failed recvfrom");
            return { Utils::Parse::Status::Err, DNSParser::DNSPtr(nullptr) }; 
        }

        // validate host
        if(from.sin_addr.s_addr != m_upstream.sin_addr.s_addr ||
            from.sin_port != m_upstream.sin_port) {
            std::cerr << "DNSResolver::resolve: Unknown sender" << std::endl;
            continue; 
        }

        auto [ok, pkt] = DNSParser::deserialize(answer, recSize);
        if(ok != Utils::Parse::Status::Ok) {
            continue;
        }

        // this packet is not our, skip
        if(ldns_pkt_id(pkt.get()) != id) {
            continue;
        }
        return {Utils::Parse::Status::Ok, std::move(pkt)};
    }
}

/*static*/
int DNSResolver::makeSocket() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0) {
        perror("DNSResolver::DNSResolver: Socket failed");
        throw std::runtime_error("Socket failed");
    }

    // for block receive if server is unreacheble
    timeval tv{};
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("DNSResolver::DNSResolver: Setsockopt failed");
        close(sock);
        throw std::runtime_error("Setsockopt failed");
    }
    return sock;
}