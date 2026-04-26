#include "scanner_engine.h"
#include "html_parser.h"
#include <curl/curl.h>
#include <iostream>

// Callback функция для получения данных от CURL
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t total_size = size * nmemb;
    userp->append(static_cast<char*>(contents), total_size);
    return total_size;
}

ScannerEngine::ScannerEngine(ScanConfig config, DataStorage& storage)
    : config(config), storage(storage) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

ScannerEngine::~ScannerEngine() {
    stop();
    curl_global_cleanup();
}

void ScannerEngine::start() {
    if (running.load()) {
        return;  // Уже запущен
    }
    
    running = true;
    
    // Добавляем начальные URL из конфигурации
    for (const auto& url : config.seed_urls) {
        addUrl(url);
    }
    
    // Создаем рабочие потоки
    for (uint16_t i = 0; i < config.thread_count; ++i) {
        workers.emplace_back(&ScannerEngine::worker_loop, this);
    }
}

void ScannerEngine::stop() {
    if (!running.load()) {
        return;  // Уже остановлен
    }
    
    running = false;
    cv.notify_all();
    
    for (auto& t : workers) {
        if (t.joinable()) {
            t.join();
        }
    }
    workers.clear();
}

void ScannerEngine::addUrl(const std::string& url) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    
    // Проверяем, не посещали ли уже этот URL
    {
        std::lock_guard<std::mutex> visited_lock(visited_mutex);
        if (visited_urls.find(url) != visited_urls.end()) {
            return;
        }
        visited_urls.insert(url);
    }
    
    url_queue.push({url, 0});
    cv.notify_one();
}

size_t ScannerEngine::getFoundProxiesCount() const {
    return storage.count();
}

void ScannerEngine::worker_loop() {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return;
    }
    
    while (running.load()) {
        std::pair<std::string, uint16_t> url_pair;
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            
            // Ждем появления URL в очереди или сигнала остановки
            cv.wait_for(lock, std::chrono::milliseconds(100), 
                [this] { return !url_queue.empty() || !running.load(); });
            
            if (!running.load() && url_queue.empty()) {
                break;
            }
            
            if (url_queue.empty()) {
                continue;
            }
            
            url_pair = url_queue.front();
            url_queue.pop();
        }
        
        const std::string& url = url_pair.first;
        uint16_t depth = url_pair.second;
        
        fetch_and_parse(curl, url);
        
        processed_count++;
    }
    
    curl_easy_cleanup(curl);
}

std::string ScannerEngine::fetchUrl(CURL* curl, const std::string& url) {
    std::string response;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, config.request_timeout_ms);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, config.request_timeout_ms);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "MTProtoProxyScanner/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // Отключаем проверку SSL для простоты
    
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        // Логирование ошибки (в реальном проекте использовать spdlog)
        std::cerr << "Failed to fetch " << url << ": " << curl_easy_strerror(res) << std::endl;
        return "";
    }
    
    // Проверка HTTP кода ответа
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    
    if (http_code != 200) {
        std::cerr << "HTTP error " << http_code << " for " << url << std::endl;
        return "";
    }
    
    return response;
}

void ScannerEngine::fetch_and_parse(CURL* curl, const std::string& url) {
    std::string content = fetchUrl(curl, url);
    
    if (content.empty()) {
        return;
    }
    
    // Извлекаем прокси из контента
    auto proxies = HtmlParser::extractProxies(content, url);
    
    if (!proxies.empty()) {
        storage.append(proxies);
        std::cout << "Found " << proxies.size() << " proxies on " << url << std::endl;
    }
    
    // Если еще не достигнута максимальная глубина, извлекаем ссылки
    // Примечание: в данной реализации depth не используется полноценно
    // для рекурсивного обхода, но структура готова к расширению
    auto links = HtmlParser::extractLinks(content, url);
    
    for (const auto& link : links) {
        // Добавляем только URL с тех же доменов (упрощенная проверка)
        if (link.substr(0, 4) == "http") {
            addUrl(link);
        }
    }
}

void ScannerEngine::processLinks(const std::vector<std::string>& links, 
                                  const std::string& current_url, uint16_t depth) {
    // Эта функция может быть использована для более умной обработки ссылок
    // с учетом глубины обхода и фильтрации по доменам
}
