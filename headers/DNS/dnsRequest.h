#ifndef DNSREQUEST_H
#define DNSREQUEST_H

#include <iostream>
#include <stdint.h>

struct DNSRequest {
    std::string domain;
    uint16_t type;
    uint16_t id;
};

#endif // DNSREQUEST_H