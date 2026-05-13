#include "../../headers/Net/dnsServer.h"

DNSServer::DNSServer(DNSDispatcher& disp) : m_dispatcher(disp) {
    m_serv.sin_family = AF_INET;
    m_serv.sin_port = htons(UDP_DNS_PORT);
    m_serv.sin_addr.s_addr = htonl(INADDR_ANY);

    m_startTime = std::chrono::steady_clock::now();

    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(m_socket  < 0) {
        perror("DNSServer::DNSServer: Socket failed");
        throw std::runtime_error("Socket failed");
    }

    if(bind(m_socket, reinterpret_cast<sockaddr*>(&m_serv), sizeof(m_serv)) < 0) {
        perror("DNSServer::DNSServer: Bind failed");
        close(m_socket);
        m_socket = -1;
        throw std::runtime_error("Bind failed");
    }

    m_sender = std::thread([this]() {
        senderLoop();
    });
}

bool DNSServer::isRunning() const {
    return !m_stop;
}

double DNSServer::getStartTime() const {
    return m_stop ? double{} : std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - m_startTime).count();
}

uint64_t DNSServer::getTotalRequests() const {
    return m_threadPool.getTotalRequests();
}

double DNSServer::getLatency() {
    return m_threadPool.getLatency();
}

double DNSServer::getErrors() const {
    return m_threadPool.getErrors();
}

bool DNSServer::run() {
    if(m_socket < 0) {
        perror("DNSServer::run: Socket closed");
        return false;
    }

    std::cout << "DNSServer::run: starting..." << std::endl;
    while(!m_stop) {
        OutPacket::Packet packet;
        packet.data.resize(BUFFER_SIZE);

        socklen_t len = sizeof(packet.client);

        // get receive
        ssize_t sizePacket = recvfrom(m_socket, packet.data.data(), packet.data.size(), 
            0, reinterpret_cast<sockaddr*>(&packet.client), &len);

        if(sizePacket < 0) {
            perror("DNSServer::run: Recvfrom failed");
            continue;
        }

        packet.size = sizePacket;
        packet.data.resize(sizePacket);

        // create dnsTask
        std::shared_ptr<Task> newTask = std::make_shared<DNSTask>(m_dispatcher, std::move(packet)); 

        m_threadPool.enqueue(newTask);

    }
    return true;
}

void DNSServer::senderLoop() {
    while(true) {
        std::optional<OutPacket::Packet> packet = m_threadPool.popResult();
        if(!packet) {
            break;
        }

        auto& receive = packet.value();

        // send answer
        ssize_t senSize = sendto(m_socket, receive.data.data(), receive.size, 0, 
                    reinterpret_cast<sockaddr*>(&receive.client), sizeof(receive.client));
                    
        if(senSize <= 0) {
            perror("DNSServer::senderLoop: Failed sendto");
        }
    }
}

DNSServer::~DNSServer() {
    m_stop = true;
    m_threadPool.stopQueue();

    if(m_socket >= 0) {
        shutdown(m_socket, SHUT_RDWR);
        close(m_socket);
    }
    
    if(m_sender.joinable()) {
        m_sender.join();
    }
}
