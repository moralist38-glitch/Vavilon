#pragma once

#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include "scanner_engine.h"
#include "data_storage.h"

class MainWindow : public QWidget {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
private slots:
    void onStartClicked();
    void onStopClicked();
    void onUpdateTable();
    
private:
    void setupUI();
    void updateStatus(const QString& status);
    
    ScannerEngine* engine;
    DataStorage storage;
    ScanConfig config;
    
    QPushButton* btnStart;
    QPushButton* btnStop;
    QTableWidget* table;
    QLabel* statusLabel;
    QProgressBar* progressBar;
    QTimer* updateTimer;
};
