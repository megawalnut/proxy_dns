#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <cstdint>
#include <arpa/inet.h>

struct Packet {
    std::vector<uint8_t> data;
    std::size_t size = 0;
    sockaddr_in client;
};

#endif // PACKET_H 