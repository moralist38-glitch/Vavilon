#pragma once

#include <string>
#include <cstdint>
#include <chrono>
#include <vector>

struct ProxyEntry {
    std::string ip;
    uint16_t port;
    std::string secret;  // для MTProto
    std::string source_url;
    std::chrono::system_clock::time_point found_at;
    bool validated = false;
};

struct ScanConfig {
    std::vector<std::string> seed_urls;
    uint16_t max_depth = 2;
    uint16_t thread_count = 4;
    uint16_t request_timeout_ms = 5000;
};
