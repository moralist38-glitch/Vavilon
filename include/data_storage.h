#pragma once

#include <vector>
#include <string>
#include <mutex>
#include "types.h"

class DataStorage {
public:
    // Добавление прокси в хранилище
    void append(const std::vector<ProxyEntry>& proxies);
    
    // Получение всех сохраненных прокси
    std::vector<ProxyEntry> getAll() const;
    
    // Сохранение в текстовый файл
    bool saveToFile(const std::string& filename) const;
    
    // Загрузка из текстового файла
    bool loadFromFile(const std::string& filename);
    
    // Очистка хранилища
    void clear();
    
    // Количество сохраненных прокси
    size_t count() const;
    
private:
    mutable std::mutex storage_mutex;
    std::vector<ProxyEntry> proxies;
};
