#include "proxy_validator.h"
#include <curl/curl.h>
#include <cstring>
#include <iostream>

bool ProxyValidator::validate(const ProxyEntry& proxy, uint16_t timeout_ms) {
    return sendMtProtoHandshake(proxy.ip, proxy.port, proxy.secret, timeout_ms);
}

bool ProxyValidator::sendMtProtoHandshake(const std::string& ip, uint16_t port, 
                                           const std::string& secret, uint16_t timeout_ms) {
    // Создаем CURL для TCP соединения
    CURL* curl = curl_easy_init();
    if (!curl) {
        return false;
    }
    
    bool success = false;
    
    // Устанавливаем таймаут
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout_ms);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, timeout_ms);
    
    // Формируем URL для проверки соединения (не HTTP, а просто TCP check)
    // Для MTProto нужна специальная проверка рукопожатия
    // Здесь упрощенная версия - только проверка TCP соединения
    
    std::string url = "http://" + ip + ":" + std::to_string(port) + "/";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    
    // Не следуем редиректам и не получаем тело ответа
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 0L);
    
    // Пытаемся соединиться
    CURLcode res = curl_easy_perform(curl);
    
    if (res == CURLE_OK || res == CURLE_HTTP_RETURNED_ERROR) {
        // Соединение установлено (даже если HTTP ошибка - это нормально для MTProto)
        success = true;
    }
    
    // TODO: Полноценная проверка MTProto рукопожатия требует:
    // 1. Отправки специального пакета с секретом
    // 2. Получения ответа от сервера
    // 3. Проверки корректности ответа
    // Это требует реализации протокола MTProto, что выходит за рамки данного примера
    
    curl_easy_cleanup(curl);
    return success;
}
