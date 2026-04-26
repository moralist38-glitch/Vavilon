#pragma once

#include <string>
#include <vector>
#include "types.h"

class HtmlParser {
public:
    // Извлечение всех ссылок из HTML
    static std::vector<std::string> extractLinks(const std::string& html, const std::string& base_url);
    
    // Поиск паттернов прокси (ip:port:secret) в тексте
    static std::vector<ProxyEntry> extractProxies(const std::string& content, const std::string& source_url);
    
    // Раскодирование Base64
    static std::string decodeBase64(const std::string& encoded);
    
    // Парсинг JSON ответа API
    static std::vector<ProxyEntry> parseJsonProxies(const std::string& json, const std::string& source_url);
};
