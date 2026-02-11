#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QString>
#include <QHBoxLayout>
#include <QFont>
#include <QGridLayout>
#include "mainwindow.h"
#include "levelpage.h"
#include <QColor>
#include <QMessageBox>
#include <QRect>


MainWindow::MainWindow(QWidget* parent):QMainWindow(parent){
//主窗口设置 
    resize(1600,1000);
    stack=new QStackedWidget(this);
    setCentralWidget(stack);
//创建
    createMenuPage();
    level1=new LevelPage(1);
    level2=new LevelPage(2);
    level3=new LevelPage(3);
    level4=new LevelPage(4);
    QVector<LevelPage*> levelstack{level1,level2, level3, level4};

//已闯关信息
    save.setFileName("save.txt");
    passinfo=readPassing();

//加入容器
    stack->addWidget(menupage);
    for (LevelPage* level:levelstack){
        stack->addWidget(level);
        connect(level,&LevelPage::returnToMenu,this,[this,level](){
            stack->setCurrentIndex(0);
            level->doStop();
        });
        connect(level,&LevelPage::restartGame,this,[this,level](){restartGame(level);});
        connect(level,&LevelPage::pass, this, [this,level](){savePassing(level->levelNumber());});
    }
}
void MainWindow::restartGame(LevelPage* level){
    int id=level->levelNumber();
    stack->setCurrentIndex(0);
    QMetaObject::invokeMethod(this,[=](){
        stack->removeWidget(level);
        level->deleteLater();
        LevelPage* fresh=new LevelPage(id);
        connect(fresh,&LevelPage::returnToMenu,this,[=](){
            stack->setCurrentIndex(0);
            level->doStop();
    });
        connect(fresh,&LevelPage::restartGame,this,[=](){MainWindow::restartGame(fresh);});
        connect(fresh,&LevelPage::pass,this, [=](){savePassing(id);});
        stack->insertWidget(id,fresh);
        stack->setCurrentIndex(id);
    },Qt::QueuedConnection);
}
//创建主页
void MainWindow::createMenuPage(){
    menupage=new QWidget;
    menupage->setStyleSheet("background-color:rgb(84,153,93);");
    QGridLayout* grid=new QGridLayout(menupage);

    QLabel* title=new QLabel("Xiaoming's Machine");
    title->setFont(QFont("Consolas",75));
    title->setStyleSheet("font-weight:bold;color:rgb(255,255,255);");

    QLabel* chooselevel = new QLabel ("Choose your Level!");
    chooselevel->setStyleSheet("color:rgb(255,255,255);");
    chooselevel->setFont(QFont("Consolas", 40));
    grid->addWidget(title,0,0,1,4,Qt::AlignCenter);
    grid->addWidget(chooselevel,2,0,1,4,Qt::AlignCenter);
    levelwar.setWindowTitle("Warning");
    levelwar.setText("Please complete the previous levels first");
    levelwar.setFixedSize(600,200);
    levelwar.setStyleSheet(
        "QLabel{min-width:500px;min-height:300px;font-size:24px;"
        "qproperty-alignment:'AlignCenter';}"
        "QPushButton{min-width:140px;min-height:44px;font-size:18px;}"
    );
    levelwar.addButton("OK",QMessageBox::AcceptRole);
    for (int i=1;i<5;i++){
        QPushButton* btn=new QPushButton(QString("%1").arg(i));
        btn->setFixedSize(100,100);
        btn->setStyleSheet("background-color:rgb(232,181,35);color:rgba(4, 4, 4, 1);");
        btn->setFont(QFont("Consolas",40));
        grid->addWidget(btn,1,i-1);
        QObject::connect(btn, &QPushButton::clicked,this,[this,i](){
            if (i==1||passinfo[i-2]==1){
                stack->setCurrentIndex(i);
            }
            else levelwar.exec();
        });
    }
    grid->setAlignment(Qt::AlignCenter);
    grid->setVerticalSpacing(200);
    grid->setHorizontalSpacing(20);
}

void MainWindow::savePassing(int id){
    passinfo[id-1]=1;
    if (save.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate)){
        QTextStream out(&save);
        for (int i=0;i<4;i++){
            out<<passinfo[i]<<' ';
        }
        save.close();
    }
}
QVector<int> MainWindow::readPassing(){
    QVector<int> v(4);
    if (save.open(QIODevice::ReadOnly|QIODevice::Text)){
        QTextStream in(&save);
        for (int i=0;i<4&&!in.atEnd();i++){
            in>>v[i];
        }
        save.close();
    }
    return v;
}