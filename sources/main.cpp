#include "../headers/Net/dnsServer.h"
#include "../headers/DNS/dnsResolver.h"
#include "../headers/DNS/dnsDispatcher.h"
#include "../headers/Metrics/metricsManager.h"
#include "../headers/Net/clients.h"

static constexpr inline const char* DNS_RESOLVER = "77.88.8.8";

int main(int argc, const char* argv[]) {
    Clients clients;
    DNSCache cache;
    DNSResolver resolver(DNS_RESOLVER);
    DNSDispatcher dispatcher(cache, resolver);
    DNSServer server(dispatcher);
    MetricsManager metrics(server, resolver, dispatcher, clients);

    if(!server.run()) {
        perror("Main::main: Failed to start server");
        return -1;
    }
    return 0;
}