# MTProto Proxy Scanner

Программа для сканирования и сбора MTProto прокси из открытых источников с графическим интерфейсом.

![GUI Preview](https://via.placeholder.com/800x600.png?text=MTProto+Proxy+Scanner+GUI)

## 🖥️ Графический интерфейс

Программа оснащена удобным GUI на базе Qt, который включает:

- **Кнопка "Старт"** — запуск многопоточного сканирования
- **Кнопка "Стоп"** — безопасная остановка сканирования
- **Таблица результатов** — отображение найденных прокси в реальном времени:
  - IP адрес
  - Порт
  - Секретный ключ
  - Источник обнаружения
  - Статус проверки (рабочий/не проверен)
- **Строка статуса** — информация о прогрессе сканирования
- **Индикатор прогресса** — визуальное отображение активности

### Скриншот интерфейса

```
┌─────────────────────────────────────────────────────────┐
│  MTProto Proxy Scanner                           [─][□][×]│
├─────────────────────────────────────────────────────────┤
│  [Старт]  [Стоп]                                        │
│  ─────────────────────────────────────────────────────  │
│  Статус: Обработано URL: 15 | Найдено прокси: 7        │
│  ████████████████████████░░░░░░░░░░ 60%                │
│  ─────────────────────────────────────────────────────  │
│  IP          │ Порт │ Секрет      │ Источник │ Статус  │
│  ─────────────────────────────────────────────────────  │
│  192.168.1.1 │ 8080 │ abc123...   │ github.. │ ✓ Рабочий│
│  10.0.0.5    │ 443  │ fed789...   │ t.me/... │ ○ Не пр. │
│  ...         │ ...  │ ...       │ ...      │ ...     │
│                                                       │
└─────────────────────────────────────────────────────────┘
```

## 📥 Установка и запуск

### Способ 1: Скачивание с GitHub (рекомендуется)

#### Шаг 1: Клонирование репозитория

```bash
# Клонируйте репозиторий
git clone https://github.com/yourusername/mtproto-proxy-scanner.git

# Перейдите в директорию проекта
cd mtproto-proxy-scanner
```

#### Шаг 2: Установка зависимостей

**Для Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install -y cmake g++ libcurl4-openssl-dev qtbase5-dev
# или для Qt6:
# sudo apt install -y cmake g++ libcurl4-openssl-dev qt6-base-dev
```

**Для Fedora/RHEL:**
```bash
sudo dnf install -y cmake gcc-c++ libcurl-devel qt5-qtbase-devel
# или для Qt6:
# sudo dnf install -y cmake gcc-c++ libcurl-devel qt6-qtbase-devel
```

**Для macOS:**
```bash
brew install cmake curl qt@5
# или для Qt6:
# brew install cmake curl qt
```

**Для Windows:**

1. Установите [Visual Studio Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/)
2. Установите [CMake](https://cmake.org/download/)
3. Установите [Qt](https://www.qt.io/download-open-source)
4. Установите [libcurl для Windows](https://curl.se/windows/)

#### Шаг 3: Сборка проекта

```bash
# Создание директории сборки
mkdir build && cd build

# Конфигурация с GUI (Qt)
cmake .. -DWITH_GUI=ON

# Сборка
cmake --build . --config Release

# Запуск
./MTProtoProxyScanner
# На Windows:
# Release\MTProtoProxyScanner.exe
```

### Способ 2: Скачать готовый бинарный файл

Перейдите на страницу [Releases](https://github.com/yourusername/mtproto-proxy-scanner/releases) и скачайте готовую сборку для вашей операционной системы:

- **Windows**: `MTProtoProxyScanner-windows-x64.zip`
- **macOS**: `MTProtoProxyScanner-macos-x64.dmg`
- **Linux**: `MTProtoProxyScanner-linux-x64.tar.gz`

Распакуйте архив и запустите исполняемый файл.

### Способ 3: Docker (альтернативный)

```bash
# Сборка образа
docker build -t mtproto-scanner .

# Запуск с пробросом X11 (Linux/macOS)
docker run -it --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix mtproto-scanner
```

## 🏗️ Архитектура

### Основные компоненты:

- **ScannerEngine** — многопоточный движок сканирования
- **HtmlParser** — извлечение данных из HTML/JavaScript
- **ProxyValidator** — проверка работоспособности найденных прокси
- **DataStorage** — хранение результатов в памяти и файле
- **MainWindow** — GUI на Qt

## Используемые библиотеки

| Назначение | Библиотека |
|------------|------------|
| HTTP-запросы | libcurl |
| GUI | Qt 5/6 (виджеты) |
| HTML-парсинг | std::regex (встроенный) |
| Многопоточность | std::thread, std::mutex, std::atomic |

## 🔧 Сборка из исходного кода

### Требования

- CMake 3.16+
- C++17 компилятор (GCC 8+, Clang 7+, MSVC 2019+)
- Qt 5.12+ или Qt 6
- libcurl

### Пошаговая инструкция

```bash
# Создание директории сборки
mkdir build && cd build

# Конфигурация проекта с GUI
cmake .. -DWITH_GUI=ON

# Сборка
cmake --build . --config Release

# Запуск
./MTProtoProxyScanner
```

## 📁 Структура проекта

```
.
├── CMakeLists.txt          # Файл сборки
├── include/
│   ├── types.h             # Основные структуры данных
│   ├── scanner_engine.h    # Движок сканирования
│   ├── html_parser.h       # Парсер HTML
│   ├── proxy_validator.h   # Валидатор прокси
│   ├── data_storage.h      # Хранилище данных
│   └── mainwindow.h        # Главное окно GUI
└── src/
    ├── main.cpp            # Точка входа
    ├── mainwindow.cpp      # Реализация GUI
    ├── scanner_engine.cpp  # Реализация движка
    ├── html_parser.cpp     # Реализация парсера
    ├── proxy_validator.cpp # Реализация валидатора
    └── data_storage.cpp    # Реализация хранилища
```

## Принцип работы

1. **Загрузка начальных URL** — список источников, где публикуются MTProto прокси
2. **Рекурсивный обход** — HTTP GET через libcurl, извлечение всех ссылок
3. **Извлечение прокси** — поиск паттернов `ip:port:secret` через регулярные выражения
4. **Валидация** — установка TCP-соединения и проверка доступности
5. **Сохранение** — запись в текстовый файл `proxies.txt`

## ⚙️ Управление

- **Старт** — запуск многопоточного сканирования
- **Стоп** — безопасная остановка сканирования с сохранением результатов

## 📝 Формат прокси

Программа распознает следующие форматы:

- `IP:PORT:SECRET` — прямой формат
- `tg://proxy?server=IP&port=PORT&secret=SECRET` — Telegram ссылки
- JSON с полями `ip`, `port`, `secret`

## 📄 Пример результата

Файл `proxies.txt`:

```
192.168.1.1:8080:abcdef1234567890  # source: https://example.com
10.0.0.1:443:fedcba0987654321    # source: https://another.com
```

## 📜 Лицензия

MIT

---

## ❓ Часто задаваемые вопросы

### Как добавить свои источники для сканирования?
Откройте файл `src/mainwindow.cpp` и отредактируйте список `config.seed_urls`, добавив нужные URL.

### Почему некоторые прокси не проходят валидацию?
Прокси могут быть недоступны по нескольким причинам:
- Сервер прокси offline
- Прокси требует аутентификацию
- Сетевые ограничения (файрвол, блокировки)

### Можно ли увеличить скорость сканирования?
Да, измените параметр `config.thread_count` в файле `src/mainwindow.cpp` (по умолчанию 4 потока).

### Где сохраняются результаты?
Результаты сохраняются в файл `proxies.txt` в текущей директории после остановки сканирования.

## 🤝 Вклад в проект

1. Fork репозиторий
2. Создайте ветку (`git checkout -b feature/amazing-feature`)
3. Закоммитьте изменения (`git commit -m 'Add amazing feature'`)
4. Отправьте в удалённый репозиторий (`git push origin feature/amazing-feature`)
5. Откройте Pull Request

## 📬 Контакты

- GitHub Issues: [Сообщить о проблеме](https://github.com/yourusername/mtproto-proxy-scanner/issues)
- Email: example@example.com