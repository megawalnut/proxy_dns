#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
    #include <ws2tcpip.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#ifdef _WIN32
    #define CLOSESOCK closesocket
    typedef SOCKET sock_t;
    typedef int rec_t;
#else
    #define CLOSESOCK close
    typedef int sock_t;
    typedef ssize_t rec_t;
#endif

#include <iostream>
#include "../headers/DNS/dnsParser.h"
#include "../headers/DNS/dnsDispatcher.h"

constexpr int UDP_DNS_PORT = 53;
constexpr int QUEUE_SIZE = 10;
constexpr int BUFFER_SIZE = 1024;

int main(int argc, const char* argv[]) {
    DNSParser parser;
    DNSDispatcher dispatcher;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_DNS_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed\n" << std::endl;
        return 1;
    }

    sock_t DNSSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(DNSSocket  == INVALID_SOCKET) {
        std::cout << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    if (bind(DNSSocket, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cout << "bind failed: " << WSAGetLastError() << std::endl;
        CLOSESOCK(DNSSocket);
        WSACleanup();
        return 1;
    }
#endif  
#ifdef __linux__
    sock_t DNSSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(DNSSocket  < 0) {
        perror("Main::main: Socket failed");
        return 1;
    }

    if (bind(DNSSocket, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Main::main: Bind failed");
        CLOSESOCK(DNSSocket);
        return 1;
    }
#endif

    std::cout << "Main::main: Starting..." << std::endl;

    while(true) {
        uint8_t buffer[BUFFER_SIZE];

        sockaddr_in client{};
        socklen_t len = sizeof(client);

        rec_t sizePacket = recvfrom(
            DNSSocket, 
            buffer, 
            BUFFER_SIZE, 
            0, 
            (sockaddr*)&client, 
            &len
        );

        if(sizePacket < 0) {
            perror("Main::main: Recvfrom failed");
            continue;
        }

        if (sizePacket == 0) {
            perror("Main::main: Empty packet");
            continue;
        }

        // parsing to DNS packet
        auto [ok, baseDNSPacket] = parser.parse(buffer, sizePacket);
        if(ok != DNSParser::ParseStatus::ParseOk) {
            perror("Main::main: Recvfrom failed");
            continue;
        }

        // get responces and dispatch
        dispatcher.dispatch(baseDNSPacket);

        printf("Receive packet: %ld bytes\n", static_cast<long>(sizePacket));
        for (rec_t i = 0; i < sizePacket; ++i) {
            printf("%02x ", static_cast<unsigned char>(buffer[i]));
        }
        printf("\n");
    }

    CLOSESOCK(DNSSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}