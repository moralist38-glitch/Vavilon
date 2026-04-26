#include "mainwindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <iostream>

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent), engine(nullptr), updateTimer(new QTimer(this)) {
    
    // Настройка конфигурации по умолчанию
    config.thread_count = 4;
    config.max_depth = 2;
    config.request_timeout_ms = 5000;
    
    // Начальные URL для сканирования (примеры реальных источников)
    config.seed_urls = {
        "https://github.com/mtgproxy/proxies",
        "https://t.me/mtp_proxy",
        "https://t.me/MTProxy"
    };
    
    setupUI();
    
    // Таймер для обновления таблицы
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::onUpdateTable);
    updateTimer->start(1000);  // Обновление каждую секунду
}

MainWindow::~MainWindow() {
    if (engine) {
        delete engine;
    }
}

void MainWindow::setupUI() {
    setWindowTitle("MTProto Proxy Scanner");
    resize(800, 600);
    
    auto* layout = new QVBoxLayout(this);
    
    // Панель с кнопками
    auto* buttonLayout = new QHBoxLayout();
    
    btnStart = new QPushButton("Старт");
    btnStop = new QPushButton("Стоп");
    btnStop->setEnabled(false);
    
    buttonLayout->addWidget(btnStart);
    buttonLayout->addWidget(btnStop);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    // Статус бар
    statusLabel = new QLabel("Готов к работе");
    layout->addWidget(statusLabel);
    
    // Прогресс бар
    progressBar = new QProgressBar();
    progressBar->setRange(0, 0);  // Бесконечный режим
    progressBar->setValue(0);
    layout->addWidget(progressBar);
    
    // Таблица с прокси
    table = new QTableWidget(0, 5);
    table->setHorizontalHeaderLabels({"IP", "Порт", "Секрет", "Источник", "Статус"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setAlternatingRowColors(true);
    
    layout->addWidget(table);
    
    // Подключение сигналов к слотам
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(btnStop, &QPushButton::clicked, this, &MainWindow::onStopClicked);
}

void MainWindow::onStartClicked() {
    if (engine && engine->isRunning()) {
        return;
    }
    
    // Очищаем старые данные
    storage.clear();
    table->setRowCount(0);
    
    // Создаем и запускаем движок
    engine = new ScannerEngine(config, storage);
    engine->start();
    
    btnStart->setEnabled(false);
    btnStop->setEnabled(true);
    progressBar->setEnabled(true);
    
    updateStatus("Сканирование запущено...");
}

void MainWindow::onStopClicked() {
    if (!engine || !engine->isRunning()) {
        return;
    }
    
    engine->stop();
    
    btnStart->setEnabled(true);
    btnStop->setEnabled(false);
    progressBar->setEnabled(false);
    
    updateStatus("Сканирование остановлено");
    
    // Сохраняем результаты в файл
    if (storage.count() > 0) {
        storage.saveToFile("proxies.txt");
        QMessageBox::information(this, "Готово", 
            QString("Найдено %1 прокси. Результаты сохранены в proxies.txt")
                .arg(storage.count()));
    }
}

void MainWindow::onUpdateTable() {
    if (!engine) {
        return;
    }
    
    auto proxies = storage.getAll();
    
    // Обновляем таблицу только если есть изменения
    if (static_cast<size_t>(table->rowCount()) != proxies.size()) {
        table->setRowCount(proxies.size());
        
        for (size_t i = 0; i < proxies.size(); ++i) {
            const auto& proxy = proxies[i];
            
            table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(proxy.ip)));
            table->setItem(i, 1, new QTableWidgetItem(QString::number(proxy.port)));
            table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(proxy.secret)));
            
            // Сокращаем длинный URL
            QString source = QString::fromStdString(proxy.source_url);
            if (source.length() > 50) {
                source = source.left(47) + "...";
            }
            table->setItem(i, 3, new QTableWidgetItem(source));
            
            QString status = proxy.validated ? "✓ Рабочий" : "○ Не проверен";
            table->setItem(i, 4, new QTableWidgetItem(status));
        }
        
        // Прокрутка к последней строке
        if (table->rowCount() > 0) {
            table->scrollToBottom();
        }
    }
    
    // Обновляем статус
    QString status = QString("Обработано URL: %1 | Найдено прокси: %2")
        .arg(engine->getProcessedCount())
        .arg(storage.count());
    
    if (engine->isRunning()) {
        status += " | Статус: Сканирование...";
    }
    
    updateStatus(status);
}

void MainWindow::updateStatus(const QString& status) {
    statusLabel->setText(status);
}
