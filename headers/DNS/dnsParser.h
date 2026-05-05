#ifndef DNSPARSER_H
#define DNSPARSER_H

#include <iostream>
#include <memory>
#include <vector>
#include <ldns/ldns.h>

#include "../Common/utils.h"

class DNSParser final {
public:
    //deleter form ldns_packet
    struct PktDeleter {
        void operator()(ldns_pkt* pkt) const {
            if(pkt) {
                ldns_pkt_free(pkt);
            }
        }
    };

    using DNSPtr = std::unique_ptr<ldns_pkt, PktDeleter>;
    using DNSPkt = std::pair<Utils::Parse::Status, DNSPtr>;
    using DNSWire = std::pair<Utils::Parse::Status, std::vector<uint8_t>>;

    static DNSPkt deserialize(const std::vector<uint8_t>& packet, const std::size_t size);
    static DNSWire serialize(const DNSPtr& packet);
};

#endif // DNSPARSER_H