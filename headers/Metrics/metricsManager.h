#ifndef METRICSMANAGER_H
#define METRICSMANAGER_H

#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <atomic>
#include <fstream>
#include <sys/resource.h>
#include <nlohmann/json.hpp>
#include <boost/asio.hpp>

#include "../../headers/Net/dnsServer.h"
#include "../../headers/DNS/dnsResolver.h"
#include "../../headers/DNS/dnsDispatcher.h"
#include "../../headers/Common/utils.h"
#include "../../headers/Common/keys.h"

using namespace Utils;

class MetricsManager final {
public: 
    struct Snapshot {
        // ToolBar
        bool running {};
        double uptime_sec {};
        uint32_t requests_sec {};
        double cache_hit {};
        double latency_p95 {};
        double resolve_avg {};
        double errors {};

        // Central Data
        std::vector<MetricRecords::TopDomainRecord> top_domains;
        std::vector<MetricRecords::QuerryTypeRecord> querry_types;
        std::vector<MetricRecords::ErrorRecord> recent_errors;

        // StatusBar
        uint64_t cache_entries {};
        uint64_t total_requests {};
        uint32_t ram {};
        double cpu {};
        uint32_t threads {};
    };

public:
    MetricsManager(DNSServer& server, DNSResolver& resolv, DNSDispatcher& disp);
    ~MetricsManager();

private:
    void startCollection();
    static nlohmann::json toJSON(const Snapshot& sn);
    Snapshot createSnapshot();
    void sendSnapshot(const nlohmann::json& js) const;

    double getCPULoad() const;
    uint32_t getStatusMetric(const std::string& metric) const;
    

private:
    std::mutex m_mtx;
    std::thread m_collector;
    
    // for calculating
    std::atomic<uint64_t> m_prev_requests {};

    DNSServer& m_server;
    DNSResolver& m_resolver;
    DNSDispatcher& m_dispatcher;

    boost::asio::io_context m_io;
    boost::asio::steady_timer m_timer;
};

#endif // METRICSMANAGER_H