#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QWheelEvent>
#include <algorithm>
#include <QVBoxLayout>
#include <QString>
#include <QInputDialog>
#include <QRect>
#include <QFont>
#include <QColor>
#include <QString>
#include <functional>
#include <QHBoxLayout>
#include <QVector>
#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <QAbstractButton>
#include <QPoint>
#include <QTimer>
#include <QTextEdit>
#include <QShowEvent>
#include <QResizeEvent>
#include <QMessageBox>
#include <QPropertyAnimation>
#pragma once
using namespace std;
enum Instruction{inbox,outbox,add,sub,copyto,copyfrom,jump,jumpifzero};
class LevelPage;
using Op=int(LevelPage::*)(int);
//数字方块
struct Block{
    explicit Block(QRect r,int n,bool blank):rect(r),num(n),isblank(blank){}
    Block():rect(),num(0),isblank(true){};
    //void outboxBlock(QRect rect);
    QRect rect;
    int num;
    bool isblank;
};
//函数方块
struct FunctionBlock{
    QRect rect;
    QString name;
    QColor color;
    Op func;
    bool isTemplate;
    double canshu;
    FunctionBlock(QRect rect,QString name,QColor c,Op func,bool templ=true):rect(rect),name(name),color(c),func(func),isTemplate(templ){}
};
//关卡信息
struct LevelInfo{
    int id;
    int spacen;
    QVector<int> input;
    QVector<int> target_output;
    QVector<Instruction> allowedins;
    QString introduction;
};

class Robot;

//关卡页
class LevelPage: public QWidget{
    Q_OBJECT
    public:
        explicit LevelPage(int id,QWidget* parent=nullptr);
        int levelNumber() const {return id;}
        int Inbox(int i);
        int Outbox(int i);
        int Add(int i);
        int Sub(int i);
        int Copyto(int i);
        int Copyfrom(int i);
        int Jump(int ins);
        int Jumpifzero(int ins);
    protected:
        void paintEvent(QPaintEvent*) override;
        void mousePressEvent(QMouseEvent*) override;
        void mouseMoveEvent(QMouseEvent*) override;
        void mouseReleaseEvent(QMouseEvent*) override;
        void wheelEvent(QWheelEvent* e) override;
        void resizeEvent(QResizeEvent* e) override;
        void showEvent(QShowEvent* e) override;

    public slots: 
        void runSequence();
        void doOneStep();
        void doStop();
    signals:
        void returnToMenu();
        void restartGame();
        void pass();
    private:
        QInputDialog dlg;
        bool dialogOpen=false;
        QPushButton * returnbtn;
        QPushButton* restartbtn;
        LevelInfo info;
        int id;
        //QRect spaceTray(inputRect.right()+20)
        QRect inputRect,outputRect,targetRect,funcPlace;
        QVector<FunctionBlock> placedBlock,funcblocks,allowed;
        QVector<Block> inputarr,outputarr,spacearr;
        QVector<Op> sequenceOrder;
        QVector<int> sequenceOrderCanshu,target_output;
        QVector<Instruction> allowedin;
        int draggingIndex=-1;
        QPoint draggingOffset;
        bool running{false};
        QTimer *timer{nullptr};
        QLabel* selectcanshu;
        int insptr=0;
        int ptrinput=0;
        bool numext=false;
        int nownum;
        QMessageBox msgBox;
        Robot* robot{nullptr};
        bool check(Op func);
        bool result();
        int numberinput;
        int rectlen;
        int spacenumber;
        void outGame(QString text);
        void drawNumBlock(QPainter& p, Block& b);
        void drawFuncBlock(QPainter& p, FunctionBlock& b);
        QPropertyAnimation* animo;
        QAbstractButton* btnHome{nullptr};
        QAbstractButton* btnRestart{nullptr};
        int selectedSlot=-1;
        int targetScroll=0;
    };



