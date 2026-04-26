#pragma once

#include <string>
#include "types.h"

class ProxyValidator {
public:
    // Проверка работоспособности прокси через TCP соединение и MTProto рукопожатие
    static bool validate(const ProxyEntry& proxy, uint16_t timeout_ms = 5000);
    
private:
    // Отправка тестового пакета MTProto
    static bool sendMtProtoHandshake(const std::string& ip, uint16_t port, 
                                      const std::string& secret, uint16_t timeout_ms);
};
