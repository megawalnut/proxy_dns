#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <chrono>
#include <nlohmann/json.hpp>
#include <ldns/ldns.h>
#include <arpa/inet.h>

#include "keys.h"

namespace Utils {
    namespace DNS {
        enum class ClassTypes : uint32_t {
            Unknown = 0,
            Internet
        };

        enum class Types : uint32_t {
            Unknown = 0,
            A,
            AAAA,
            MX,
            TXT,
            CNAME,
            NS
        };

        static inline std::string typeToStr(Types type) {
            switch(type) {
                case Types::A:     return "A";
                case Types::AAAA:  return "AAAA";
                case Types::MX:    return "MX";
                case Types::TXT:   return "TXT";
                case Types::CNAME: return "CNAME";
                case Types::NS:    return "NS";
                default:           return "UNKNOWN";
            }
        }

        static inline Types fromDNSType(ldns_rr_type type) {
            switch(type) {
                case LDNS_RR_TYPE_A:     return Types::A;
                case LDNS_RR_TYPE_AAAA:  return Types::AAAA;
                case LDNS_RR_TYPE_MX:    return Types::MX;
                case LDNS_RR_TYPE_TXT:   return Types::TXT;
                case LDNS_RR_TYPE_CNAME: return Types::CNAME;
                case LDNS_RR_TYPE_NS:    return Types::NS;
                default:                 return Types::Unknown;
            };
        }

        static inline ldns_rr_type toDNSType(Types type) {
            switch(type) {
                case Types::A:       return LDNS_RR_TYPE_A;
                case Types::AAAA:    return LDNS_RR_TYPE_AAAA;
                case Types::MX:      return LDNS_RR_TYPE_MX;
                case Types::TXT:     return LDNS_RR_TYPE_TXT;
                case Types::CNAME:   return LDNS_RR_TYPE_CNAME;
                case Types::NS:      return LDNS_RR_TYPE_NS;
                case Types::Unknown:
                default:             return LDNS_RR_TYPE_ANY;
            };
        }

        static inline ClassTypes fromDNSClassType(ldns_rr_class type) {
            switch(type) {
                case LDNS_RR_CLASS_IN: return ClassTypes::Internet;
                default:               return ClassTypes::Unknown;
            };
        }
        
        static inline ldns_rr_class toDNSClassType(ClassTypes type) {
            switch(type) {
                case ClassTypes::Internet: return LDNS_RR_CLASS_IN;
                case ClassTypes::Unknown:
                default:                   return LDNS_RR_CLASS_ANY;
            };
        }
    };

    namespace Cache {
        struct Key {
            std::string domain;
            DNS::Types type = DNS::Types::Unknown;

            bool operator ==(const Key& other) const {
                return domain == other.domain && type == other.type;
            }
        };

        struct Record {
            Cache::Key key;
            DNS::ClassTypes classType;
            uint32_t ttl {};
            std::vector<std::string> rdata;
            std::chrono::steady_clock::time_point created;
        };
    };

    namespace Parse {
        enum class Status : uint32_t {
            Ok = 0,
            Err
        };
    };

    namespace MetricRecords {
        struct TopDomainRecord {
            std::string domain;
            double percent {};
            uint64_t requests {};
        };
        //@override
        static inline void to_json(nlohmann::json& j, const TopDomainRecord& rec) {
            j = nlohmann::json{
                {Keys::TOP_DOMAINS_DOMAIN,  rec.domain},
                {Keys::TOP_DOMAINS_PERCENT, rec.percent},
                {Keys::TOP_DOMAINS_REQUEST, rec.requests}
            };
        }

        struct QueryTypeRecord {
            std::string type;
            double percent {};
        };
        //@override
        static inline void to_json(nlohmann::json& j, const QueryTypeRecord& rec) {
            j = nlohmann::json{
                {Keys::QUERY_TYPES_TYPE,    rec.type},
                {Keys::QUERY_TYPES_PERCENT, rec.percent}
            };
        }

        struct ErrorRecord {
            double time;
            std::string domain;
            std::string error;
        };
        //@override
        static inline void to_json(nlohmann::json& j, const ErrorRecord& rec) {
            j = nlohmann::json{
                {Keys::RECENT_ERRORS_TIME, rec.time},
                {Keys::RECENT_ERRORS_DOMAIN, rec.domain},
                {Keys::RECENT_ERRORS_ERROR, rec.error}
            };
        }
    };

    namespace OutPacket {
        struct Packet {
            std::vector<uint8_t> data;
            std::size_t size = 0;
            sockaddr_in client;
        };
    };
};

#endif // UTILS_H