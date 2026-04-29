#ifndef DNSSERVER_H
#define DNSSERVER_H

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "dnsParser.h"
#include "dnsDispatcher.h"
#include "../utils.h"

class DNSServer final {
    static constexpr std::size_t BUFFER_SIZE = 2048;
    static constexpr int UDP_DNS_PORT = 53;
    static constexpr int QUEUE_SIZE = 10;

public:
    DNSServer(DNSDispatcher& disp);
    ~DNSServer();

    bool run();

private:
    int m_socket = -1;
    sockaddr_in m_serv;
    DNSDispatcher& m_dispatcher;
};

#endif // DNSSERVER_H