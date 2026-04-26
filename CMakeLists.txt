cmake_minimum_required(VERSION 3.16)
project(MTProtoProxyScanner VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опция для включения GUI (требуется Qt)
option(WITH_GUI "Build with Qt GUI" OFF)

# Поиск CURL всегда нужен
find_package(CURL REQUIRED)

# Исходные файлы (базовые - без GUI)
set(SOURCES
    src/scanner_engine.cpp
    src/html_parser.cpp
    src/proxy_validator.cpp
    src/data_storage.cpp
    src/main_console.cpp
)

# Заголовочные файлы (базовые)
set(HEADERS
    include/scanner_engine.h
    include/html_parser.h
    include/proxy_validator.h
    include/data_storage.h
    include/types.h
)

# Если включен GUI, добавляем файлы Qt
if(WITH_GUI)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    
    # Поиск пакетов
    find_package(Qt6 COMPONENTS Widgets QUIET)
    if(NOT Qt6_FOUND)
        find_package(Qt5 5.12 REQUIRED COMPONENTS Widgets)
        set(QT_VERSION_MAJOR 5)
    else()
        set(QT_VERSION_MAJOR 6)
    endif()
    
    list(APPEND SOURCES
        src/main.cpp
        src/mainwindow.cpp
    )
    list(APPEND HEADERS
        include/mainwindow.h
    )
endif()

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Подключение заголовочных файлов
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
)

# Подключение библиотек
target_link_libraries(${PROJECT_NAME} PRIVATE
    CURL::libcurl
)

if(WITH_GUI)
    if(QT_VERSION_MAJOR EQUAL 6)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt6::Widgets
        )
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt5::Widgets
        )
    endif()
endif()

# Компиляционные флаги
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -pedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Установка
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
