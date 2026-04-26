# MTProto Proxy Scanner - Docker Image
FROM ubuntu:22.04

# Установка зависимостей
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    libcurl4-openssl-dev \
    qtbase5-dev \
    libqt5widgets5 \
    && rm -rf /var/lib/apt/lists/*

# Создание рабочей директории
WORKDIR /app

# Копирование исходного кода
COPY . .

# Создание директории сборки и компиляция
RUN mkdir -p build && \
    cd build && \
    cmake .. -DWITH_GUI=ON && \
    cmake --build . --config Release

# Установка переменной окружения для DISPLAY
ENV DISPLAY=:0

# Запуск приложения
CMD ["./build/MTProtoProxyScanner"]
