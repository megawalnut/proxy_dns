#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../headers/DNS/dnsParser.h"
#include "../headers/DNS/dnsDispatcher.h"
#include "../headers/DNS/dnsResolver.h"
#include "../headers/utils.h"

static constexpr int UDP_DNS_PORT = 53;
static constexpr int QUEUE_SIZE = 10;
static constexpr std::size_t BUFFER_SIZE = 2048;
static constexpr const char* DNS_RESOLVER = "8.8.8.8";

using namespace Utils;

int main(int argc, const char* argv[]) {
    DNSCache cache;
    DNSParser parser;
    DNSResolver resolver(DNS_RESOLVER);
    DNSDispatcher dispatcher(parser, cache, resolver);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_DNS_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    int DNSSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(DNSSocket  < 0) {
        perror("Main::main: Socket failed");
        return 1;
    }

    if (bind(DNSSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("Main::main: Bind failed");
        close(DNSSocket);
        return 1;
    }

    std::cout << "Main::main: Starting..." << std::endl;

    while(true) {
        uint8_t buffer[BUFFER_SIZE];

        sockaddr_in client{};
        socklen_t len = sizeof(client);

        ssize_t sizePacket = recvfrom(
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

        //processing...
        dispatcher.dispatch(buffer, sizePacket);

        printf("Receive packet: %ld bytes\n", static_cast<long>(sizePacket));
        for (ssize_t i = 0; i < sizePacket; ++i) {
            printf("%02x ", static_cast<unsigned char>(buffer[i]));
        }
        printf("\n");
    }

    close(DNSSocket);
    return 0;
}