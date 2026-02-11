#include "robot.h"
#include <QPainter>
#include <QPropertyAnimation>


Robot::Robot(QWidget* parent):QWidget(parent),m_pos(210,300),numext(false),velocity(1500){
    anim=new QPropertyAnimation(this,"robotpos");
    anim->setDuration(velocity);
    anim->setEasingCurve(QEasingCurve::Linear);
}
void Robot::stop(){
    if (anim) anim->stop();
}
void Robot::start(){
    if (!anim) return;
    if (!anim->endValue().isValid()) return;
    if (anim) anim->start();
}
void Robot::setPos(QPoint p){
    m_pos=p;
    update();
}
void Robot::clearNum(){
    numext=false;
    update();
}
void Robot::changeNum(int nownum){
    num=nownum;
    numext=true;
    update();
}
void Robot::setVelocity(int v){
    velocity=v;
    if (anim) anim->setDuration(velocity);
}

void Robot::moveTo(QPoint p){
    if (!anim) return;
    anim->stop();
    anim->setDuration(velocity);
    anim->setEasingCurve(QEasingCurve::Linear);
    anim->setStartValue(m_pos);
    anim->setEndValue(p);
    anim->start();
    //m_pos=p;
}
void Robot::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(m_pos);

    //身体
    QRect body(0,80,80,80);
    p.setBrush(QColor(150,200,255));
    p.drawRect(body);

    //头
    p.setBrush(QColor(255,220,120));
    p.drawEllipse(0,0,80,80);
    //手脚
    QPen pen(Qt::black,4);
    p.setPen(pen);
    p.drawLine(-20,60,0,90);
    p.drawLine(-20,180,0,150);
    p.drawLine(100,60,80,90);
    p.drawLine(100,180,80,150);

    //方块
    if (numext){
        QRect rect(0,-80,80,80);
        p.setBrush(QColor(40,180,90));
        p.setFont(QFont("Consolas",30,QFont::Bold));
        p.drawRect(rect);
        p.drawText(rect,Qt::AlignCenter,QString::number(num));
    }
}

