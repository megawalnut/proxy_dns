#include "../../headers/DNS/dnsResolver.h"

DNSResolver::DNSResolver(const std::string& addr/*8.8.8.8*/) {
    m_resolve.reserve(RESOLVE_SIZE);
    m_upstream.sin_family = AF_INET;
    m_upstream.sin_port = htons(UDP_DNS_PORT);
    if(inet_pton(AF_INET, addr.c_str(), &m_upstream.sin_addr) != 1) {
        throw std::invalid_argument("Failed resolving address");
    }
}

void DNSResolver::addResolve(std::chrono::nanoseconds duration) {
    std::lock_guard lock{ m_mtxRes };
    if(m_index < RESOLVE_SIZE)
        m_resolve.push_back(duration);
    else
        m_resolve[m_index % RESOLVE_SIZE] = duration;   // push_front
    ++m_index;
}

double DNSResolver::getResolve() {
    std::lock_guard lock{ m_mtxRes };
    if(m_resolve.empty()) {
        return {};
    }
    
    auto acc = std::accumulate(m_resolve.cbegin(), m_resolve.cend(), std::chrono::nanoseconds{});  // average

    double res = std::chrono::duration<double, std::milli>(acc / m_resolve.size()).count();   // ms

    m_resolve.clear();
    m_index = 0;

    return res;
}

DNSResolver::Packet DNSResolver::resolve(const DNSParser::DNSPtr& packet) {
    auto start = std::chrono::steady_clock::now();
    Packet res;

    thread_local int sock = makeSocket();
    if(sock < 0) {
        perror("DNSResolver::resolve: Socket closed");
        res.error = "FORMERR";
        return res;
    }

    // remember for get exactly my packet
    const uint16_t id = ldns_pkt_id(packet.get());

    // sserialize ldns packet to wire bytes 
    const auto& [ok, question] = DNSParser::serialize(packet);
    if(ok != Parse::Status::Ok) {
        res.error = "FORMERR";
        return res;
    }

    // send to remote host
    ssize_t senSize = sendto(sock, question.data(), question.size(), 0, 
            reinterpret_cast<sockaddr*>(&m_upstream), sizeof(m_upstream));

    if(senSize <= 0) {
        perror("DNSResolver::resolve: Failed sendto");
        res.error = "SERVFAIL";
        return res;
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
            res.error = "TIMEOUT";
            return res;
        }

        // validate host
        if(from.sin_addr.s_addr != m_upstream.sin_addr.s_addr ||
            from.sin_port != m_upstream.sin_port) {
            std::cerr << "DNSResolver::resolve: Unknown sender" << std::endl;
            continue; 
        }

        auto [ok, pkt] = DNSParser::deserialize(answer, recSize);
        if(ok != Parse::Status::Ok) {
            continue;
        }

        // this packet is not our, skip
        if(ldns_pkt_id(pkt.get()) != id) {
            continue;
        }
        
        addResolve(std::chrono::steady_clock::now() - start);
        return {Parse::Status::Ok, std::move(pkt), ""};
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