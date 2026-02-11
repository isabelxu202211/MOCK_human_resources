#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QString>
#include <QHBoxLayout>
#include "mainwindow.h"
#include "levelpage.h"



int main(int argc, char* argv[]){
    QApplication app(argc,argv);
    MainWindow w;
    w.show();
    return app.exec();
}
    