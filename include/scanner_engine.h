#pragma once

#include <atomic>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_set>
#include "types.h"
#include "data_storage.h"

class ScannerEngine {
public:
    explicit ScannerEngine(ScanConfig config, DataStorage& storage);
    ~ScannerEngine();
    
    // Запуск сканирования
    void start();
    
    // Остановка сканирования
    void stop();
    
    // Проверка состояния (работает ли сканер)
    bool isRunning() const { return running.load(); }
    
    // Добавление URL в очередь
    void addUrl(const std::string& url);
    
    // Получение количества обработанных URL
    size_t getProcessedCount() const { return processed_count.load(); }
    
    // Получение количества найденных прокси
    size_t getFoundProxiesCount() const;

private:
    void worker_loop();
    void fetch_and_parse(void* curl, const std::string& url);
    std::string fetchUrl(void* curl, const std::string& url);
    void processLinks(const std::vector<std::string>& links, const std::string& current_url, uint16_t depth);
    
    ScanConfig config;
    DataStorage& storage;
    
    std::atomic<bool> running{false};
    std::vector<std::thread> workers;
    std::mutex queue_mutex;
    std::queue<std::pair<std::string, uint16_t>> url_queue;  // url + depth
    std::condition_variable cv;
    
    std::mutex visited_mutex;
    std::unordered_set<std::string> visited_urls;
    
    std::atomic<size_t> processed_count{0};
};
