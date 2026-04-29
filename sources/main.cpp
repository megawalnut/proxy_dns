#include <iostream>

#include "../headers/DNS/dnsServer.h"
#include "../headers/DNS/dnsResolver.h"
#include "../headers/DNS/dnsDispatcher.h"

static constexpr const char* DNS_RESOLVER = "8.8.8.8";

int main(int argc, const char* argv[]) {
    DNSCache cache;
    DNSResolver resolver(DNS_RESOLVER);
    DNSDispatcher dispatcher(cache, resolver);
    DNSServer server(dispatcher);

    if(!server.run()) {
        perror("Main::main: Failed to start server");
        return -1;
    }
    return 0;
}