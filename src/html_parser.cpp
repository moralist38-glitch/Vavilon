#include "html_parser.h"
#include <regex>
#include <iostream>

std::vector<std::string> HtmlParser::extractLinks(const std::string& html, const std::string& base_url) {
    std::vector<std::string> links;
    
    // Простой regex для извлечения ссылок из href="..."
    std::regex link_regex(R"(href\s*=\s*["']([^"']+)["'])");
    auto begin = std::sregex_iterator(html.begin(), html.end(), link_regex);
    auto end = std::sregex_iterator();
    
    for (auto it = begin; it != end; ++it) {
        std::string link = (*it)[1].str();
        
        // Пропускаем якоря и javascript ссылки
        if (link.empty() || link[0] == '#' || link.substr(0, 10) == "javascript") {
            continue;
        }
        
        // Преобразуем относительные ссылки в абсолютные
        if (link.substr(0, 4) != "http") {
            if (link[0] == '/') {
                // Абсолютный путь относительно домена
                size_t domain_end = base_url.find('/', 8); // после http:// или https://
                if (domain_end != std::string::npos) {
                    link = base_url.substr(0, domain_end) + link;
                } else {
                    link = base_url + link;
                }
            } else {
                // Относительный путь относительно текущей страницы
                size_t last_slash = base_url.rfind('/');
                if (last_slash != std::string::npos && last_slash > 7) {
                    link = base_url.substr(0, last_slash + 1) + link;
                }
            }
        }
        
        links.push_back(link);
    }
    
    return links;
}

std::vector<ProxyEntry> HtmlParser::extractProxies(const std::string& content, const std::string& source_url) {
    std::vector<ProxyEntry> proxies;
    
    // Паттерн для MTProto прокси: IP:PORT:SECRET
    // IP: 1-3 цифры.1-3 цифры.1-3 цифры.1-3 цифры
    // PORT: 1-5 цифр
    // SECRET: hex строка (обычно 32 или 64 символа)
    std::regex proxy_regex(R"((\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}):(\d{1,5}):([a-fA-F0-9]{32,64}))");
    
    auto begin = std::sregex_iterator(content.begin(), content.end(), proxy_regex);
    auto end = std::sregex_iterator();
    
    for (auto it = begin; it != end; ++it) {
        ProxyEntry entry;
        entry.ip = (*it)[1].str();
        entry.port = static_cast<uint16_t>(std::stoi((*it)[2].str()));
        entry.secret = (*it)[3].str();
        entry.source_url = source_url;
        entry.found_at = std::chrono::system_clock::now();
        entry.validated = false;
        
        proxies.push_back(entry);
    }
    
    // Также пробуем найти прокси в формате tg://proxy?server=IP&port=PORT&secret=SECRET
    std::regex tg_proxy_regex(R"(tg://proxy\?[^"]*server=([^&\s]+)[^"]*port=(\d+)[^"]*secret=([a-fA-F0-9]+))");
    auto tg_begin = std::sregex_iterator(content.begin(), content.end(), tg_proxy_regex);
    auto tg_end = std::sregex_iterator();
    
    for (auto it = tg_begin; it != tg_end; ++it) {
        ProxyEntry entry;
        entry.ip = (*it)[1].str();
        entry.port = static_cast<uint16_t>(std::stoi((*it)[2].str()));
        entry.secret = (*it)[3].str();
        entry.source_url = source_url;
        entry.found_at = std::chrono::system_clock::now();
        entry.validated = false;
        
        proxies.push_back(entry);
    }
    
    return proxies;
}

std::string HtmlParser::decodeBase64(const std::string& encoded) {
    static const std::string base64_chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    std::string decoded;
    int val = 0, valb = -6;
    
    for (char c : encoded) {
        if (c == '=') break;
        auto pos = base64_chars.find(c);
        if (pos == std::string::npos) continue;
        
        val = (val << 6) + static_cast<int>(pos);
        valb += 6;
        if (valb >= 0) {
            decoded.push_back(static_cast<char>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    
    return decoded;
}

std::vector<ProxyEntry> HtmlParser::parseJsonProxies(const std::string& json, const std::string& source_url) {
    std::vector<ProxyEntry> proxies;
    
    // Простой парсинг JSON без внешних библиотек
    // Ищем поля ip, port, secret в JSON
    
    std::regex ip_regex(R"delim(\"ip\"\s*:\s*\"(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})\")delim");
    std::regex port_regex(R"delim(\"port\"\s*:\s*(\d+))delim");
    std::regex secret_regex(R"delim(\"secret\"\s*:\s*\"([a-fA-F0-9]+)\")delim");
    
    auto ip_begin = std::sregex_iterator(json.begin(), json.end(), ip_regex);
    auto ip_end = std::sregex_iterator();
    auto port_begin = std::sregex_iterator(json.begin(), json.end(), port_regex);
    auto port_end = std::sregex_iterator();
    auto secret_begin = std::sregex_iterator(json.begin(), json.end(), secret_regex);
    auto secret_end = std::sregex_iterator();
    
    // Собираем прокси, сопоставляя найденные значения
    while (ip_begin != ip_end && port_begin != port_end && secret_begin != secret_end) {
        ProxyEntry entry;
        entry.ip = (*ip_begin)[1].str();
        entry.port = static_cast<uint16_t>(std::stoi((*port_begin)[1].str()));
        entry.secret = (*secret_begin)[1].str();
        entry.source_url = source_url;
        entry.found_at = std::chrono::system_clock::now();
        entry.validated = false;
        
        proxies.push_back(entry);
        
        ++ip_begin;
        ++port_begin;
        ++secret_begin;
    }
    
    return proxies;
}
