#include <iostream>
#include <thread>
#include <chrono>
#include "scanner_engine.h"
#include "data_storage.h"

int main() {
    std::cout << "MTProto Proxy Scanner (Console Version)" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Настройка конфигурации
    ScanConfig config;
    config.thread_count = 4;
    config.max_depth = 2;
    config.request_timeout_ms = 5000;
    
    // Тестовые URL (замените на реальные источники прокси)
    config.seed_urls = {
        "https://example.com/proxy-list",
        "https://example.org/mtproto-proxies"
    };
    
    DataStorage storage;
    
    // Загрузка ранее сохраненных прокси
    if (storage.loadFromFile("proxies.txt")) {
        std::cout << "Загружено " << storage.count() << " прокси из файла" << std::endl;
    }
    
    ScannerEngine engine(config, storage);
    
    std::cout << "Запуск сканирования..." << std::endl;
    engine.start();
    
    // Работаем 30 секунд или пока пользователь не прервет
    auto start_time = std::chrono::steady_clock::now();
    const int max_runtime_seconds = 30;
    
    try {
        while (engine.isRunning()) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
            
            std::cout << "\rОбработано URL: " << engine.getProcessedCount() 
                      << " | Найдено прокси: " << storage.count()
                      << " | Время: " << elapsed << "с" << std::flush;
            
            if (elapsed >= max_runtime_seconds) {
                std::cout << "\nДостигнут лимит времени (" << max_runtime_seconds << "с)" << std::endl;
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    } catch (const std::exception& e) {
        std::cerr << "\nОшибка: " << e.what() << std::endl;
    }
    
    // Остановка сканера
    std::cout << "\nОстановка сканирования..." << std::endl;
    engine.stop();
    
    // Сохранение результатов
    if (storage.count() > 0) {
        if (storage.saveToFile("proxies.txt")) {
            std::cout << "Результаты сохранены в proxies.txt" << std::endl;
            
            // Вывод найденных прокси
            std::cout << "\nНайденные прокси:" << std::endl;
            auto proxies = storage.getAll();
            for (const auto& proxy : proxies) {
                std::cout << "  " << proxy.ip << ":" << proxy.port << ":" << proxy.secret.substr(0, 16) << "..." 
                          << " (source: " << proxy.source_url << ")" << std::endl;
            }
        } else {
            std::cerr << "Ошибка при сохранении результатов" << std::endl;
        }
    } else {
        std::cout << "Прокси не найдены" << std::endl;
    }
    
    std::cout << "\nГотово!" << std::endl;
    return 0;
}
