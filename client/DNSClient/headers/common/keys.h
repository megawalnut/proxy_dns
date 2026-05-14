#ifndef KEYS_H
#define KEYS_H

namespace Keys {
    static constexpr inline const char* UPTIME = "uptime";
    static constexpr inline const char* RUNNING = "running";
    static constexpr inline const char* REQUESTS_SEC = "request_sec";
    static constexpr inline const char* CACHE_HITS = "cache_hit";
    static constexpr inline const char* LATENCY_P95 = "latency_p95";
    static constexpr inline const char* RESOLVE = "resolve_avg";
    static constexpr inline const char* ERRORS = "errors";
    static constexpr inline const char* TOP_DOMAINS = "top_domains";
    static constexpr inline const char* QUERY_TYPES = "query_types";
    static constexpr inline const char* RECENT_ERRORS = "recent_errors";
    static constexpr inline const char* CACHE_ENTRIES = "cache_entries";
    static constexpr inline const char* TOTAL_REQUESTS = "total_requests";
    static constexpr inline const char* RAM = "ram";
    static constexpr inline const char* CPU = "cpu";
    static constexpr inline const char* THREADS = "threads";

    // additional for custom types to json 
    static constexpr inline const char* TOP_DOMAINS_DOMAIN  = "top_domains_domain";
    static constexpr inline const char* TOP_DOMAINS_PERCENT = "top_domains_percent";
    static constexpr inline const char* TOP_DOMAINS_REQUEST = "top_domains_request";

    static constexpr inline const char* QUERY_TYPES_TYPE    = "query_types_type";
    static constexpr inline const char* QUERY_TYPES_PERCENT = "query_types_percent";

    static constexpr inline const char* RECENT_ERRORS_TIME = "recent_errors_time";
    static constexpr inline const char* RECENT_ERRORS_DOMAIN = "recent_errors_domain";
    static constexpr inline const char* RECENT_ERRORS_ERROR = "recent_errors_error";
    
};

#endif  // KEYS_H
