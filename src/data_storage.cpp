#include "data_storage.h"
#include <fstream>
#include <sstream>
#include <iostream>

void DataStorage::append(const std::vector<ProxyEntry>& proxies) {
    std::lock_guard<std::mutex> lock(storage_mutex);
    for (const auto& proxy : proxies) {
        // Проверяем на дубликаты
        bool exists = false;
        for (const auto& existing : proxies) {
            if (existing.ip == proxy.ip && existing.port == proxy.port && 
                existing.secret == proxy.secret) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            this->proxies.push_back(proxy);
        }
    }
}

std::vector<ProxyEntry> DataStorage::getAll() const {
    std::lock_guard<std::mutex> lock(storage_mutex);
    return proxies;
}

bool DataStorage::saveToFile(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(storage_mutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& proxy : proxies) {
        file << proxy.ip << ":" << proxy.port << ":" << proxy.secret 
             << "  # source: " << proxy.source_url << "\n";
    }
    
    file.close();
    return true;
}

bool DataStorage::loadFromFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(storage_mutex);
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Пропускаем комментарии и пустые строки
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Удаляем комментарий в конце строки если есть
        size_t comment_pos = line.find("  #");
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        
        // Парсим формат IP:PORT:SECRET
        std::istringstream iss(line);
        std::string ip, port_str, secret;
        
        if (std::getline(iss, ip, ':') && 
            std::getline(iss, port_str, ':') && 
            std::getline(iss, secret, ':')) {
            
            ProxyEntry entry;
            entry.ip = ip;
            entry.port = static_cast<uint16_t>(std::stoi(port_str));
            entry.secret = secret;
            entry.source_url = "file:" + filename;
            entry.found_at = std::chrono::system_clock::now();
            entry.validated = false;
            
            proxies.push_back(entry);
        }
    }
    
    file.close();
    return true;
}

void DataStorage::clear() {
    std::lock_guard<std::mutex> lock(storage_mutex);
    proxies.clear();
}

size_t DataStorage::count() const {
    std::lock_guard<std::mutex> lock(storage_mutex);
    return proxies.size();
}
