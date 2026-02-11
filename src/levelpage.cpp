#include "levelpage.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QTextEdit>
#include <QTimer>
#include <QMouseEvent>
#include <QShowEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QAbstractButton>
#include <QPropertyAnimation>
#include "robot.h"
using namespace std;

QVector<Op> op={&LevelPage::Inbox,&LevelPage::Outbox,&LevelPage::Add,&LevelPage::Sub,&LevelPage::Copyto,&LevelPage::Copyfrom,&LevelPage::Jump,&LevelPage::Jumpifzero};
static const QVector<LevelInfo> levels={//关卡，空地数，input，output，ins
    {1,0,{1,2},{1,2},{inbox,outbox},"Make the robot pick up each block from the input sequence and place it into the output sequence."},
    {2,3,{3,9,5,1,-2,-2,9,-9},{-6,6,4,-4,0,0,18,-18},{inbox,outbox,add,sub,copyto,copyfrom,jump,jumpifzero},"For every pair in the input sequence, subtract the second from the first and place the result in the output sequence; then subtract the first from the second and place that result in the output sequence. Repeat it."},
    {3,3,{6,2,7,7,-9,3,-3,-3},{7,-3},{inbox,outbox,add,sub,copyto,copyfrom,jump,jumpifzero},"Take two numbers from the input sequence at a time. If they are equal, output one of them; otherwise, discard both. Repeat until the input conveyor is empty."},
    {4,5,{6,-6,2,0},{240,-240,80,0},{inbox,outbox,copyto,copyfrom,add,jump},"Multiply each number in the input sequence by 40 and place the result into the output sequence."}
};
QVector<QString> opname={"inbox","outbox","add","sub","copyto","copyfrom","jump","jumpifzero"};

//关卡页面
LevelPage::LevelPage(int leveln, QWidget* parent):QWidget(parent),id(leveln){

    QWidget* page=new QWidget;
    page->setStyleSheet("background-color:rgb(232,137,37);");
    page->resize(1600,1000);
    //设置数字方块
    numberinput=levels[id-1].input.size();
    rectlen=2000/(3*numberinput+1);
    spacenumber=levels[id-1].spacen;
    target_output=levels[id-1].target_output;
    spacearr.resize(spacenumber);
    allowedin=levels[id-1].allowedins;
    for (int i=0;i<numberinput;i++){
        Block block=Block(QRect(40,rectlen/2+i*rectlen,80,80),levels[id-1].input[i],false);
        inputarr.push_back(block);
    }
    for (int i=0;i<spacenumber;i++){
        Block block=Block(QRect(300,500,80,80),0,true);
        spacearr[i]=block;
    }
    for (int i=0;i<allowedin.size();i++){
        allowed.push_back(FunctionBlock(QRect(0,0,200,80),opname[allowedin[i]],QColor(150,255,150),op[allowedin[i]]));
    }


    //关卡介绍
    QLabel* label=new QLabel(QString("This is Level %1").arg(id),this);
    label->setGeometry(200,50,600,25);
    label->setFont(QFont("Microsoft YaHei",25));
    label->setFixedWidth(600);
    label->setStyleSheet("color:black;");
    label->setAlignment(Qt::AlignCenter);
    QLabel* intro=new QLabel(this);
    intro->setFont(QFont("Microsoft YaHei",16));
    intro->setGeometry(200,100,600,100);
    intro->setText(levels[id-1].introduction);
    intro->setFixedWidth(600);
    intro->setStyleSheet("color:black;");
    intro->setWordWrap(true);
    intro->setAlignment(Qt::AlignCenter);

    msgBox.setWindowTitle("Game Over");
    msgBox.setFixedSize(500,300);
    msgBox.setStyleSheet(
        "QLabel{min-width:500px;min-height:300px;font-size:24px;"
        "qproperty-alignment:'AlignCenter';}"
        "QPushButton{min-width:180px;min-height:44px;font-size:18px;}"
    );
    btnHome=msgBox.addButton("Return To Menu",QMessageBox::ActionRole);
    btnRestart=msgBox.addButton("Restart",QMessageBox::ActionRole);

    //运行，单步模拟，连续模拟
    timer=new QTimer;
    timer->setInterval(2000);
    running=false;
    insptr=0;
    ptrinput=0;
    QPushButton* run=new QPushButton("Run",this);
    QPushButton* stop=new QPushButton("Stop",this);
    QPushButton* step=new QPushButton("One Step",this);
    run->setGeometry(460,900,100,100);
    stop->setGeometry(360,900,100,100);
    step->setGeometry(560,900,100,100);
    run->setStyleSheet("background-color:green;color:black;");
    stop->setStyleSheet("background-color:red;color:black;");
    step->setStyleSheet("background-color:yellow;color:black;");
    run->setFont(QFont("Consolas",20,QFont::Bold));
    stop->setFont(QFont("Consolas",20,QFont::Bold));
    step->setFont(QFont("Consolas",13,QFont::Bold));
    connect(run,&QPushButton::clicked,this,&LevelPage::runSequence);
    connect(stop,&QPushButton::clicked,this,&LevelPage::doStop);
    connect(step,&QPushButton::clicked,this,&LevelPage::doOneStep);
    connect(timer,&QTimer::timeout,this,&LevelPage::doOneStep);
    
    selectcanshu=new QLabel("Please select the box",this);
    dlg.setWindowTitle(tr("Parameter"));
    dlg.setLabelText(tr("Enter Parameter:"));
    dlg.setInputMode(QInputDialog::IntInput);
    dlg.setIntRange(-999,999);
    dlg.setIntStep(1);
    dlg.setIntValue(0);
    dlg.setFixedSize(500,300);
    dlg.setStyleSheet(
    "QLabel{min-width:480px; min-height:160px; font-size:22px;"
    "       qproperty-alignment:'AlignCenter';}"
    "QPushButton{min-width:120px; min-height:40px; font-size:18px;}"
    "QSpinBox{min-height:44px;font-size:20px;}"
    "QSpinBox QLineEdit{min-height:44px; font-size:20px;}"
    "QSpinBox::up-button,QSpinBox::down-button{width:28px;}"
    );


    //回到主页面
    returnbtn=new QPushButton("Return",this);
    returnbtn->setFont(QFont("Consolas",15,QFont::Bold));
    returnbtn->setFixedHeight(50);
    returnbtn->setGeometry(660,900,100,100);
    returnbtn->setStyleSheet("background-color:rgb(255,0,0);color:black;");
    QObject::connect(returnbtn,&QPushButton::clicked,this,[=](){
        emit returnToMenu();
    });
    restartbtn=new QPushButton("Restart",this);
    restartbtn->setFont(QFont("Consolas",15,QFont::Bold));
    restartbtn->setFixedHeight(50);
    restartbtn->setGeometry(660,950,100,100);
    restartbtn->setStyleSheet("background-color:rgb(255,0,0);color:black;");
    QObject::connect(restartbtn,&QPushButton::clicked,this,[=](){
        emit restartGame();
    });
    //机器人
    robot=new Robot(this);
    robot->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    robot->setAttribute(Qt::WA_TranslucentBackground,true);
    robot->setGeometry(this->rect());
    robot->show();
    robot->raise();
}

//画图
void LevelPage::paintEvent(QPaintEvent* event){
    //画数字方块
    Q_UNUSED(event);
    QPainter p(this);

    inputRect=QRect(0,0,160,height());
    targetRect=QRect(width()-300,0,300,height());
    funcPlace=QRect(targetRect.left()-300,0,300,height());
    outputRect=QRect(funcPlace.left()-160,0,160,height());

    p.setPen(Qt::black);
    p.setBrush(QColor(107,73,15));
    p.drawRect(inputRect);
    p.drawRect(outputRect);

    p.setBrush(QColor(242,221,128));
    p.drawRect(targetRect);
    p.setFont(QFont("Consolas",15,QFont::Bold));
    p.drawText(targetRect.adjusted(0,5,0,0),Qt::AlignTop|Qt::AlignHCenter,"Put Your Functions Here");
    p.drawRect(funcPlace);
    
    for (auto &b:inputarr){
        drawNumBlock(p, b);
    }
    for (auto &b:outputarr){
        drawNumBlock(p, b);
    }
    if (spacearr.size()>0){
        int n=spacearr.size();
        int total=n*80+(n-1)*40;
        QRect spaceTray(inputRect.right()+(680-total)/2-40, 700, total+80, 100);
        p.setBrush(QColor(232,180,21));
        for (int i=0;i<n;i++){
            spacearr[i].rect=QRect(spaceTray.left()+i*120+40,710,80, 80);
        }
    }
    for (auto &b:spacearr){
        drawNumBlock(p,b);
    }
    //画函数方块
    for (int i=0;i<allowed.size();++i){
        allowed[i].rect=QRect(funcPlace.left()+50,40*(i+1)+80*i,200,80);
    }
    for (auto &f:allowed) drawFuncBlock(p,f);

    for (int i=0; i<placedBlock.size();i++){
        if (i<sequenceOrder.size()){
            int x=targetRect.left()+40;
            int y=targetRect.top()+40-targetScroll+120*i;
            placedBlock[i].rect=QRect(x,y,200,80);
        }

    }
    for (int i=0;i<placedBlock.size();i++) {
        if (i==draggingIndex) continue;
        p.save();
        p.setClipRect(targetRect);
        drawFuncBlock(p,placedBlock[i]);
        p.restore();
    }
    if (draggingIndex!=-1){
        p.save();
        p.setOpacity(0.9);
        drawFuncBlock(p,placedBlock[draggingIndex]);
        p.restore();
    }
}

//画数字方块(函数)
void LevelPage::drawNumBlock(QPainter &p,Block& b){
    p.setPen(Qt::black);
    p.setBrush(QColor(40,180,90));
    p.setFont(QFont("Consolas",30,QFont::Bold));
    p.drawRect(b.rect);
    if (!b.isblank){
        p.drawText(b.rect,Qt::AlignCenter,QString::number(b.num));
    }
}
//画函数方块（函数）
void LevelPage::drawFuncBlock(QPainter& p, FunctionBlock& b){
    p.setPen(Qt::black);
    p.setBrush(b.color);
    p.drawRect(b.rect);
    p.setFont(QFont("Consolas",25,QFont::Bold));
    p.drawText(b.rect,Qt::AlignCenter,b.name);
}

//鼠标按下
void LevelPage::mousePressEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    draggingIndex=-1;
    for (int i=0;i<allowed.size();i++){
        if (allowed[i].rect.contains(event->pos())){
            FunctionBlock copy=allowed[i];
            copy.isTemplate=false;
            copy.rect.moveTopLeft(event->pos()-QPoint(copy.rect.width()/2,copy.rect.height()/2));
            placedBlock.push_back(copy);
            draggingIndex=placedBlock.size()-1;
            draggingOffset=event->pos()-placedBlock.last().rect.topLeft();
            update();
            return;
        }
    }
}
void LevelPage::mouseMoveEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    if (draggingIndex!=-1){
        placedBlock[draggingIndex].rect.moveTopLeft(event->pos()-draggingOffset);
        update();
    }
}

void LevelPage::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    if (draggingIndex==-1) return;
    FunctionBlock &b=placedBlock[draggingIndex];
    if (targetRect.contains(b.rect.center())){
        int count=sequenceOrder.size();
        int x=targetRect.left()+40;
        int y=targetRect.top()+40-targetScroll+120*count;
        b.rect.moveTo(x,y);
        sequenceOrder.push_back(b.func);
        int canshu=0;
        if (b.func==&LevelPage::Inbox||b.func==&LevelPage::Outbox){
            canshu=0;
        }
        else {
            if (dlg.exec()==QDialog::Accepted){
                canshu=dlg.intValue();
            }
            else {
                sequenceOrder.pop_back();
                placedBlock.remove(draggingIndex);
                draggingIndex=-1;
                update();
                return;
            }
        }
        sequenceOrderCanshu.push_back(canshu);
        b.color=QColor(4,250,39);
    }
    else placedBlock.remove(draggingIndex);
    draggingIndex=-1;
    int contentH_target=80+120*sequenceOrder.size();
    int maxScroll_target=max(0,contentH_target-targetRect.height());
    targetScroll=clamp(targetScroll,0,maxScroll_target);
    update();
    return;
}
void LevelPage::wheelEvent(QWheelEvent* e){
    int step=60;
    QPoint mousePt=e->position().toPoint();
    if (targetRect.contains(mousePt)){
        targetScroll=max(0, targetScroll-e->angleDelta().y()/120*step);
        update();
        return;
    }
}
void LevelPage::runSequence(){
    if (!running){
        running=true;
        timer->start();
    }
    if (robot) robot->start();
}
void LevelPage::doOneStep(){
    if (insptr<sequenceOrder.size()){
        Op func=sequenceOrder[insptr];
        double canshu=sequenceOrderCanshu[insptr];
        if (ptrinput>inputarr.size()){
            outGame(result()?"Success":"Fail");
            return;
        }
        if (!check(func)||canshu!=(int)canshu){
            outGame(QString("Error on instruction%1").arg(insptr+1));
            return;
        }
        if ((this->*sequenceOrder[insptr])((int)canshu)==1){
            outGame(QString("Error on instruction%1").arg(insptr+1));
            return;
        };
    }
    else{
        outGame(result()?"Success":"Fail");
        return;
    }
}
void LevelPage::doStop(){
    running=false;
    timer->stop();
    if (robot) robot->stop();
}
bool LevelPage::check(Op func){
    bool find=false;
    for (int i=0;i<allowed.size();i++){
        if (func==allowed[i].func){
            find=true;
            break;
        }
    }
    return find;
}

bool LevelPage::result(){

    if (outputarr.size()!=target_output.size()) return false;
    for (int i=0;i<target_output.size();i++){
        if (outputarr[i].num!=target_output[i]) return false;
    }
    return true;
}

void LevelPage::outGame(QString text){
    if (dialogOpen) return;
    doStop();
    dialogOpen=true;
    if (text=="Success"){
        emit pass();
    }
    msgBox.setText(text);
    msgBox.exec();
    QAbstractButton* clicked=msgBox.clickedButton();
    if (clicked==btnHome){
        emit returnToMenu();
    }
    else if (clicked==btnRestart){
        emit restartGame();
    }
}

//执行函数
    int LevelPage::Inbox(int i){
        Q_UNUSED(i);
        QRect src=inputarr[ptrinput].rect;
        robot->moveTo(src.topRight());
        int idx=ptrinput;
        insptr++;
        QTimer::singleShot(robot->duration(),this,[=](){
            if (idx<inputarr.size()){
                nownum=inputarr[idx].num;
                robot->changeNum(nownum);
                numext=true;
                inputarr[idx].isblank=true;
                update();
            }
        });
        ptrinput++;
        return 0;
    }
    int LevelPage::Outbox(int i){
        Q_UNUSED(i);
        if (!numext) return 1;
        int idx=outputarr.size();
        int w=80,h=80;
        int gap=40;
        int topY=outputRect.top()+100;
        int x=outputRect.left()+(outputRect.width()-w)/2;
        int y=topY+idx*h+gap*(idx-1);
        robot->moveTo(QPoint(x-80,y));
        insptr++;
        QTimer::singleShot(robot->duration(),this,[=](){
            outputarr.push_back(Block(QRect(x,y,80,80),nownum,false));
            numext=false;
            robot->clearNum();
            update();
        });
        return 0;
    }
    /*
    void Block::outboxBlock(QRect rect){
        animo=new QPropertyAnimation(&rect,"pos");
        animo->setDuration(3000);
        animo->setStartValue(880,500);
        animo->setEndValue(880,1000);
        animo->start();
    };*/

    int LevelPage::Add(int i){
        if (!numext||i<0||i>=spacenumber||spacearr[i].isblank==true) return 1;
        nownum+=spacearr[i].num;
        robot->moveTo(spacearr[i].rect.topLeft()+QPoint(0,-150));
        insptr++;
        QTimer::singleShot(robot->duration(),this,[=](){
            robot->changeNum(nownum);
        });
        return 0;
    }
    int LevelPage::Sub(int i){
        if (!numext||i<0||i>=spacearr.size()||spacearr[i].isblank) return 1;
        nownum-=spacearr[i].num;
        robot->moveTo(spacearr[i].rect.topLeft()+QPoint(0,-150));
        insptr++;
        QTimer::singleShot(robot->duration(),this,[=](){
            robot->changeNum(nownum);
        });
        return 0;
    }
    int LevelPage::Copyto(int i){
        if (!numext||i>=spacearr.size()||i<0) return 1;
        robot->moveTo(spacearr[i].rect.topLeft()+QPoint(0,-150));
        insptr++;
        QTimer::singleShot(robot->duration(),this,[=](){
            spacearr[i].num=nownum;
            spacearr[i].isblank=false;
            update();
        });
        return 0;
    }
    int LevelPage::Copyfrom(int i){
        if (i>=spacearr.size()||spacearr[i].isblank||i<0) return 1;
        robot->moveTo(spacearr[i].rect.topLeft()+QPoint(0,-150));
        insptr++;
        QTimer::singleShot(robot->duration(),this,[=](){
            nownum=spacearr[i].num;
            robot->changeNum(nownum);
            numext=true;
        });
        return 0;
    }
    int LevelPage::Jump(int ins){
        if (ins<=0||ins>sequenceOrder.size()) return 1; 
        insptr=ins-1;
        return 0;
    }
    int LevelPage::Jumpifzero(int ins){
        if (!numext||ins<=0||ins>sequenceOrder.size()) return 1;
        if (nownum==0) insptr=ins-1;
        else insptr++;
        return 0;
    }
//ROBOT
void LevelPage::resizeEvent(QResizeEvent* e){
    QWidget::resizeEvent(e);
    if (robot) {robot->setGeometry(this->rect());robot->raise();}
}

void LevelPage::showEvent(QShowEvent* e){
    QWidget:: showEvent(e);
    if (robot) robot->raise();
}
