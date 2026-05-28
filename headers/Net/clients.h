#ifndef CLIENTS_H
#define CLIENTS_H

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>

class Clients final {
    static constexpr inline int CLIENTS_PORT = 6060;
public:
    Clients();
    ~Clients();

    void erase(std::vector<int>  sock);
    std::vector<int> getClients();

private:
    void startListen();

private:
    int m_socket = -1;
    std::vector<int> m_clients;

    std::mutex m_mtx;
    std::thread m_listener;
    std::atomic<bool> m_stop{};
};

#endif // CLIENTS_H