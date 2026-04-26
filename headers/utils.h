#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <chrono>

namespace Utils {
    namespace Cache {
        struct Key {
            std::string domain;
            DNS::Types type = DNS::Types::Unknown;

            bool operator ==(const Key& other) const {
                return domain == other.domain && type == other.type;
            }
        };

        struct Record {
            std::vector<std::string> rdata;
            uint32_t ttl = 0;
            std::chrono::steady_clock::time_point created;
        };
    };

    namespace DNS {
        enum class Types : uint32_t {
            Unknown = 0,
            A,
            AAAA,
            MX,
            TXT,
            CNAME,
            NS
        };
    };

    namespace Parse {
        enum class Status : uint32_t {
            Ok = 0,
            Err
        };
    };
};

#endif // UTILS_H