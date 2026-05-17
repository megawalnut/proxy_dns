#ifndef UTILS_H
#define UTILS_H

#include <QJsonObject>
#include <QJsonArray>
#include <vector>
#include <string>

#include "keys.h"

namespace Utils {
    namespace MetricRecords {
        struct TopDomainRecord {
            std::string domain;
            double percent {};
            uint64_t requests {};
        };

        struct QueryTypeRecord {
            std::string type;
            double percent {};
        };

        struct ErrorRecord {
            double time;
            std::string domain;
            std::string error;
        };

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
            std::vector<TopDomainRecord> top_domains;
            std::vector<QueryTypeRecord> query_types;
            std::vector<ErrorRecord> recent_errors;

            // StatusBar
            uint64_t cache_entries {};
            uint64_t total_requests {};
            uint32_t ram {};
            double cpu {};
            uint32_t threads {};
        };
    }
    namespace Areas {
        struct ToolBarData {
            bool running {};
            double uptime_sec {};
            uint32_t requests_sec {};
            double cache_hit {};
            double latency_p95 {};
            double resolve_avg {};
            double errors {};
        };

        struct CentralData {
            uint32_t requests_sec {};
            double cache_hit {};
            double latency_p95 {};
            std::vector<MetricRecords::TopDomainRecord> top_domains;
            std::vector<MetricRecords::QueryTypeRecord> query_types;
            std::vector<MetricRecords::ErrorRecord> recent_errors;
        };

        struct StatusBarData {
            uint64_t cache_entries {};
            uint64_t total_requests {};
            uint32_t ram {};
            double cpu {};
            uint32_t threads {};
        };
    }

    static inline MetricRecords::Snapshot parseSnapshot(const QJsonObject& obj) {
        MetricRecords::Snapshot sn;

        // ToolBar
        sn.uptime_sec     = obj[Keys::UPTIME].toDouble();
        sn.running        = obj[Keys::RUNNING].toBool();
        sn.requests_sec   = obj[Keys::REQUESTS_SEC].toVariant().toUInt();
        sn.cache_hit      = obj[Keys::CACHE_HITS].toDouble();
        sn.latency_p95    = obj[Keys::LATENCY_P95].toDouble();
        sn.resolve_avg    = obj[Keys::RESOLVE].toDouble();
        sn.errors         = obj[Keys::ERRORS].toDouble();

        // Central Data
        auto domains = obj[Keys::TOP_DOMAINS].toArray();
        std::vector<MetricRecords::TopDomainRecord> topDomains;
        topDomains.reserve(domains.size());
        for(const auto& dom : domains) {
            const auto map = dom.toObject();
            MetricRecords::TopDomainRecord td;
            td.domain = map.value(Keys::TOP_DOMAINS_DOMAIN).toString().toStdString();
            td.percent = map.value(Keys::TOP_DOMAINS_PERCENT).toDouble();
            td.requests = map.value(Keys::TOP_DOMAINS_REQUEST).toVariant().toULongLong();
            topDomains.emplace_back(td);
        }

        sn.top_domains    = topDomains;
        auto types = obj[Keys::QUERY_TYPES].toArray();
        std::vector<MetricRecords::QueryTypeRecord> queryTypes;
        queryTypes.reserve(types.size());
        for(const auto& typ : types) {
            const auto map = typ.toObject();
            MetricRecords::QueryTypeRecord qt;
            qt.type = map.value(Keys::QUERY_TYPES_TYPE).toString().toStdString();
            qt.percent = map.value(Keys::QUERY_TYPES_PERCENT).toDouble();
            queryTypes.emplace_back(qt);
        }

        sn.query_types    = queryTypes;
        auto errs = obj[Keys::RECENT_ERRORS].toArray();
        std::vector<MetricRecords::ErrorRecord> recentErrors;
        recentErrors.reserve(errs.size());
        for(const auto& err : errs) {
            const auto map = err.toObject();
            MetricRecords::ErrorRecord er;
            er.time = map.value(Keys::RECENT_ERRORS_TIME).toDouble();
            er.domain = map.value(Keys::RECENT_ERRORS_DOMAIN).toString().toStdString();
            er.error = map.value(Keys::RECENT_ERRORS_ERROR).toString().toStdString();
            recentErrors.emplace_back(er);
        }

        // Statusbar
        sn.recent_errors    = recentErrors;
        sn.cache_entries  = obj[Keys::CACHE_ENTRIES].toVariant().toULongLong();
        sn.total_requests = obj[Keys::TOTAL_REQUESTS].toVariant().toULongLong();
        sn.ram            = obj[Keys::RAM].toVariant().toUInt();
        sn.cpu            = obj[Keys::CPU].toDouble();
        sn.threads        = obj[Keys::THREADS].toVariant().toUInt();

        return sn;
    }

    static inline Areas::ToolBarData parseToolBarData(const MetricRecords::Snapshot& sn) {
        Areas::ToolBarData td;

        td.running      = sn.running;
        td.uptime_sec   = sn.uptime_sec;
        td.requests_sec = sn.requests_sec;
        td.cache_hit    = sn.cache_hit;
        td.latency_p95  = sn.latency_p95;
        td.resolve_avg  = sn.resolve_avg;
        td.errors       = sn.errors;

        return td;
    }

    static inline Areas::CentralData parseCentralData(MetricRecords::Snapshot sn/*std::move*/) {
        Areas::CentralData cd;

        cd.requests_sec  = sn.requests_sec;
        cd.cache_hit     = sn.cache_hit;
        cd.latency_p95   = sn.latency_p95;

        cd.top_domains   = std::move(sn.top_domains);
        cd.query_types   = std::move(sn.query_types);
        cd.recent_errors = std::move(sn.recent_errors);

        return cd;
    }

    static inline Areas::StatusBarData parseStatusBarData(const MetricRecords::Snapshot& sn) {
        Areas::StatusBarData sd;

        sd.cache_entries  = sn.cache_entries;
        sd.total_requests = sn.total_requests;
        sd.ram            = sn.ram;
        sd.cpu            = sn.cpu;
        sd.threads        = sn.threads;

        return sd;
    }
}

#endif // UTILS_H
