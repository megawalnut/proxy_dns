#include "../../headers/Metrics/metricsManager.h"

MetricsManager::MetricsManager(DNSServer& server, DNSResolver& resolv, DNSDispatcher& disp)
    :
    m_server(server),
    m_resolver(resolv),
    m_dispatcher(disp),
    m_timer(m_io)
{
    startCollection();
    
    m_collector = std::thread([this]() {
        m_io.run();
    });
}

MetricsManager::~MetricsManager() {
    m_io.stop();

    if(m_collector.joinable()) {
        m_collector.join();
    }
}

void MetricsManager::startCollection() {
    m_timer.expires_after(std::chrono::seconds(1));

    m_timer.async_wait([this](const boost::system::error_code& ec) {
        if(!ec) {
            Snapshot sn = createSnapshot();
            sendSnapshot(toJSON(sn));

            startCollection();
        }
    });
}

/*static*/
nlohmann::json MetricsManager::toJSON(const Snapshot& sn) {
    nlohmann::json json;

    json[Keys::UPTIME] = sn.uptime_sec;
    json[Keys::RUNNING] = sn.running;
    json[Keys::REQUESTS_SEC] = sn.requests_sec;
    json[Keys::CACHE_HITS] = sn.cache_hit;
    json[Keys::LATENCY_P95] = sn.latency_p95;
    json[Keys::RESOLVE] = sn.resolve_avg;
    json[Keys::ERRORS] = sn.errors;
    json[Keys::TOP_DOMAINS] = sn.top_domains;
    json[Keys::QUERRY_TYPES] = sn.querry_types;
    json[Keys::RECENT_ERRORS] = sn.recent_errors;
    json[Keys::CACHE_ENTRIES] = sn.cache_entries;
    json[Keys::TOTAL_REQUESTS] = sn.total_requests;
    json[Keys::RAM] = sn.ram;
    json[Keys::CPU] = sn.cpu;
    json[Keys::THREADS] = sn.threads;

    return json;
}

MetricsManager::Snapshot MetricsManager::createSnapshot() {
    Snapshot status;

    status.running = m_server.isRunning();                  // bool
    status.uptime_sec = m_server.getStartTime();            // ms
    auto requests = m_server.getTotalRequests();
    status.requests_sec = requests - m_prev_requests;       // reqs/sec
    m_prev_requests = requests;
    status.cache_hit = m_dispatcher.getHitsPercent();       // percent
    status.latency_p95 = m_server.getLatency();             // ms
    status.resolve_avg = m_resolver.getResolve();           // ms
    status.errors = m_server.getErrors();                   // percent

    status.top_domains = m_dispatcher.getTopDomains();      // top domains
    status.querry_types = m_dispatcher.getQuerryTypes();    // querry types
    status.recent_errors = m_dispatcher.getRecentErrors();  // error record

    status.cache_entries = m_dispatcher.getCacheEntries();  // count
    status.total_requests = requests;                       // count
    status.ram = getStatusMetric("VmRSS:") / 1024;          // mb
    status.cpu = getCPULoad();                              // percent
    status.threads = getStatusMetric("Threads:");           // count

    return status;
}

void MetricsManager::sendSnapshot(const nlohmann::json& sn) const {

}

double MetricsManager::getCPULoad() const {
    struct rusage cpuTime{};
    
    if(getrusage(RUSAGE_SELF, &cpuTime) != 0) {
        std::cerr << "MetricsManager::getCPULoad: Failed to get resource usage statistics for the calling process" << std::endl;
        return {};
    }

    double timeDuration = m_server.getStartTime();  // ms
    double cpuDuration = (cpuTime.ru_utime.tv_sec  + cpuTime.ru_stime.tv_sec)  * 1000.0 +
                         (cpuTime.ru_utime.tv_usec + cpuTime.ru_stime.tv_usec) / 1000.0;   // ms

    return timeDuration > 0 ? cpuDuration / timeDuration * 100.0 : 0.0;
}

uint32_t MetricsManager::getStatusMetric(const std::string& metric) const {
    std::ifstream stat("/proc/self/status");
    std::string line;

    while(getline(stat, line)) {
        if(line.compare(0, metric.size(), metric) == 0) {
            std::string value = line.substr(metric.size());
            return std::stoul(value);
        }
    }
    return 0;
}