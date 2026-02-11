#pragma once 
#include <QWidget>
#include <QPainter>
#include <QPoint>
#include <QPropertyAnimation>

class Robot:public QWidget{
    Q_OBJECT
    Q_PROPERTY(QPoint robotpos READ pos WRITE setPos)
    public:
        explicit Robot(QWidget* parent=nullptr);
        void setPos(QPoint p);
        void changeNum(int num);
        void setVelocity(int x);
        void moveTo(QPoint p);
        QPoint pos() const{return m_pos;}
        int duration() const {return velocity;}
        void stop();
        void start();
        bool hasNum() const {return numext;}
        void clearNum();
    protected:
        void paintEvent(QPaintEvent* event) override;
    private:
        QPoint m_pos;
        int num;
        bool numext;
        int velocity;
        QPropertyAnimation* anim;
};