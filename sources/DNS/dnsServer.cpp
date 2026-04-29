#include "../../headers/DNS/dnsServer.h"

DNSServer::DNSServer(DNSDispatcher& disp) : m_dispatcher(disp) {
    m_serv.sin_family = AF_INET;
    m_serv.sin_port = htons(UDP_DNS_PORT);
    m_serv.sin_addr.s_addr = INADDR_ANY;

    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(m_socket  < 0) {
        perror("DNSServer::DNSServer: Socket failed");
        throw std::runtime_error("Socket failed");
    }

    if (bind(m_socket, reinterpret_cast<sockaddr*>(&m_serv), sizeof(m_serv)) < 0) {
        perror("DNSServer::DNSServer: Bind failed");
        close(m_socket);
        m_socket = -1;
        throw std::runtime_error("Bind failed");
    }
}

bool DNSServer::run() {
    if(m_socket < 0) {
        perror("DNSServer::run: Socket closed");
        return false;
    }

    std::cout << "DNSServer::run: starting..." << std::endl;
    while(true) {
        std::cout << "Receive packet" << std::endl;
        std::vector<uint8_t> question;
        question.resize(BUFFER_SIZE);

        sockaddr_in client{};
        socklen_t len = sizeof(client);

        ssize_t sizePacket = recvfrom(m_socket, question.data(), question.size(), 
            0, reinterpret_cast<sockaddr*>(&client), &len);

        if(sizePacket < 0) {
            perror("DNSServer::run: Recvfrom failed");
            continue;
        }

        if (sizePacket == 0) {
            perror("DNSServer::run: Empty packet");
            continue;
        }

        // parsing Q-Packet
        auto [ok, qPacket] = DNSParser::deserialize(question);
        if(ok != Parse::Status::Ok) {
            std::cerr << "DNSServer::run: Parse failed" << std::endl;
            continue;
        }

        // get A-Packet
        DNSParser::DNSPtr aPacket = m_dispatcher.dispatch(qPacket);

        std::vector<uint8_t> answer = DNSParser::serialize(aPacket);

        ssize_t senSize = sendto(m_socket, answer.data(), answer.size(), 0, 
                    reinterpret_cast<sockaddr*>(&client), sizeof(client));
        if(senSize <= 0) {
            perror("DNSServer::run: Failed sendto");
            continue; 
        }
    }
    return true;
}

DNSServer::~DNSServer() {
    if(m_socket >= 0) {
        close(m_socket);
    }
}
