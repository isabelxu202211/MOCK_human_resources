#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QString>
#include <QFile>
#include <QTextEdit>
#include <QTextStream>
#include <QHBoxLayout>
#include <QMessageBox>
#pragma once
//主页

class LevelPage;

class MainWindow: public QMainWindow{
    Q_OBJECT
    public:
    explicit MainWindow(QWidget * parent=nullptr);
    void createMenuPage();
    void restartGame(LevelPage* level);
    void savePassing(int id);
    QVector<int> readPassing();

    private:
    QStackedWidget *stack;
    QWidget* menupage;
    LevelPage* level1;
    LevelPage* level2;
    LevelPage* level3;
    LevelPage* level4;
    QFile save;
    QVector<int> passinfo;
    QMessageBox levelwar;
    
};
