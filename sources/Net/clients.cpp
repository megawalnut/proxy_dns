#include "../../headers/Net/clients.h"

Clients::Clients() {
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(CLIENTS_PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(m_socket  < 0) {
        perror("Clients::Clients: Socket failed");
        throw std::runtime_error("Socket failed");
    }

    int optVal = 1;
    if(setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) < 0) {
        perror("Clients::Clients: Setsockopt failed");
        close(m_socket);
        throw std::runtime_error("Setsockopt failed");
    }

    if(bind(m_socket, reinterpret_cast<sockaddr*>(&server), sizeof(server)) < 0) {
        perror("Clients::Clients: Bind failed");
        throw std::runtime_error("Bind failed");
    }

    m_listener = std::thread([this]() {
        startListen();
    });
}

Clients::~Clients() {
    m_stop = true;

    if(m_socket >= 0) {
        shutdown(m_socket, SHUT_RDWR);
        close(m_socket);
    }
    
    {
        std::lock_guard lock{ m_mtx };
        for(const auto& client : m_clients) {
            shutdown(client, SHUT_RDWR);
            close(client);
        }
        m_clients.clear();
    }

    if(m_listener.joinable()) {
        m_listener.join();
    }
}

void Clients::erase(std::vector<int> socks) {
    std::lock_guard lock{ m_mtx };

    for(const auto& sock : socks) {
        auto it = std::find(m_clients.cbegin(), m_clients.cend(), sock);
        
        if(it != m_clients.cend()) {
            close(sock);
            m_clients.erase(it);
        }
    }
}

std::vector<int> Clients::getClients() {
    std::lock_guard lock{ m_mtx };
    return m_clients;
}

void Clients::startListen() {
    if(m_socket < 0) {
        perror("Clients::startListen: Socket closed");
        return;
    }

    if(listen(m_socket, SOMAXCONN) < 0) {
        perror("Clients::startListen: Listen failed");
        return;
    }

    while(!m_stop) {
        sockaddr_in newClient;
        socklen_t len = sizeof(newClient);

        // get receive
        int clientSocket = accept(m_socket, reinterpret_cast<sockaddr*>(&newClient), &len);

        if(clientSocket < 0) {
            if(m_stop) {
                break;
            }
            perror("Clients::startListen: Accept failed");
            continue;
        }
        {
            std::lock_guard lock{ m_mtx };
            m_clients.push_back(clientSocket);
        }
    }
}