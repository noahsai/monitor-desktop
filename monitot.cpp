#include "monitot.h"
#include "ui_monitot.h"

monitot::monitot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::monitot)
{
    ui->setupUi(this);
    this->setGeometry(QApplication::desktop()->width()-120,QApplication::desktop()->height()-120,86,32);
    setWindowFlags(Qt::FramelessWindowHint
       |Qt::WindowStaysOnTopHint
        |Qt::Tool
     );//去边框//最ding层显示//不在任务栏显示
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    tuichu  = new QAction("退出", this);
    moniter = new QAction("系统监视器",this);
    connect(tuichu, SIGNAL(triggered(bool)), qApp, SLOT(quit()));//若触发了退出就退出程序
    connect(moniter,SIGNAL(triggered(bool)),this,SLOT(openmoniter()));

    memused =0;
    upspeed = "↑0K";
    downspeed = "↓0K";

    mousepressed=false;
    oldpos.setX(0);
    oldpos.setY(0);
    oup=0;
    odown=0;
    side = -1;
    showall = true;
    animation = new QPropertyAnimation(this, "geometry");
    connect(animation,SIGNAL(finished()),SLOT(animationfinished()));
    justpress = true;
    pro = new QProcess(this);
    killer = new monitorwindow(this);
    killer->hide();
    timer = new QTimer(this);
    timer->setInterval(1000);
    timer->setSingleShot(false);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeout()));
    timer->start();
    poscheck();
}

monitot::~monitot()
{
    delete ui;
}

void monitot::paintEvent(QPaintEvent *event)
{
    QFont font;
    QPainter paint(this);
    paint.setRenderHint(QPainter::Antialiasing,true);
    if(showall)
    {
        //===============先画底============
        QLinearGradient linearGradient(0,0,0,32);
        //创建了一个QLinearGradient对象实例，参数为起点和终点坐标，可作为颜色渐变的方向
        //painter.setPen(Qt::NoPen);
        linearGradient.setColorAt(0.0,Qt::white);
        linearGradient.setColorAt(1.0,QColor(224, 224, 224));
        paint.setBrush(QBrush(linearGradient));
        //↓由于使用了反锯齿，四边都要多1像素，需要右下各移动1像素再绘图，确保边缘平滑显示。
        paint.setPen(QPen(QColor(210, 210, 210), 1));
        //  paint.setBrush(QBrush(Qt::white));
        paint.drawRoundedRect(1,1,84,30,15,15);


        //=========================再画网速数值==================
        font.setPixelSize(11);
        paint.setFont(font);
        paint.setPen(QPen(Qt::red, 1));
        paint.drawText(QRectF(29,2,58,14),Qt::AlignLeft|Qt::AlignVCenter,upspeed);

        paint.setPen(QPen(QColor(60, 150, 21), 1));
        paint.drawText(QRectF(29,16,58,14),Qt::AlignLeft|Qt::AlignVCenter,downspeed);

    }
//===================再画内存底=========================
    QString picture;
    if(memused>=80)//内存大时的颜色！默认红色
   {
        picture = RED;
   }
    else if(memused>=65&&memused<80)//内存占用较多时的颜色！默认橙色
    {
         picture = YELLOW;
    }
    else picture = GREEN;

    int mem = memused * 30 / 100;
    QPixmap pix = QPixmap(picture).copy(0,mem,30,30);
    paint.setBrush(QBrush(Qt::green,pix));
    paint.setPen(QPen(QColor(210, 210, 210), 1));
    if(side==0&&(!showall))    paint.drawRoundedRect(-13,1,30,30,15,15);
    else if(side == 1&&(!showall)) {
        paint.translate(0,-14);
        paint.drawRoundedRect(1,1,30,30,15,15);
    }
    else paint.drawRoundedRect(1,1,30,30,15,15);

    //==================再写内存数值==============================
    if(showall)
    {
        paint.setPen(QPen(QColor(180, 180, 180), 1));
        font.setPixelSize(16);
        paint.setFont(font);
        paint.drawText(QRectF(2,1,30,30),Qt::AlignCenter,QString().setNum( memused));
        paint.setPen(QPen(QColor(255, 255, 255), 1));
        paint.drawText(QRectF(1,1,30,30),Qt::AlignCenter,QString().setNum( memused));
    }
    else {
        paint.setPen(QPen(QColor(150, 150, 150), 1));
        font.setPixelSize(12);
        paint.setFont(font);
         if(side ==0)
        {
            paint.drawText(QRectF(0,1,18,30),Qt::AlignCenter,QString().setNum( memused));
            paint.setPen(QPen(QColor(255, 255, 255), 1));
            paint.drawText(QRectF(-1,1,18,30),Qt::AlignCenter,QString().setNum( memused));
        }
        else if(side ==1)
        {
            paint.translate(0,14);
            paint.drawText(QRectF(2,-1,30,18),Qt::AlignCenter,QString().setNum( memused));
            paint.setPen(QPen(QColor(255, 255, 255), 1));
            paint.drawText(QRectF(1,-1,30,18),Qt::AlignCenter,QString().setNum( memused));
        }
        else
        {
            paint.drawText(QRectF(2,1,18,30),Qt::AlignCenter,QString().setNum( memused));
            paint.setPen(QPen(QColor(255, 255, 255), 1));
            paint.drawText(QRectF(1,1,18,30),Qt::AlignCenter,QString().setNum( memused));
        }
    }
    paint.save();
    paint.restore();
    QWidget::paintEvent(event);
}


void monitot::timeout()
{
    if(killer->isVisible()&&(!killer->isActiveWindow()))
    {
        killer->stop_hide();
    }
   if(showall) getnetspeed();
    getmemused();
    update();

}

void monitot::getnetspeed()
{
    QRegularExpressionMatchIterator matchs;
    QString result,tmp;
    qint64 up,down,t;
    upspeed = "";
    upspeed = "";
    up=0;
    down=0;
    pro->start("ip -s link");
    if(pro->waitForStarted(2000))
    {
        if(pro->waitForFinished(1000))
        {
            result=pro->readAllStandardOutput();
          //  qDebug()<<result;
            if(!result.isEmpty())
            {
                result.remove(QRegularExpression("link/(?!loopback|ether)[\\S\\s]+?collsns[\\S\\s]+?(\\d+ +){6}"));
                reg.setPattern("collsns[\\s\\S]+?\\d+");
                matchs=reg.globalMatch(result);
                while(matchs.hasNext())
                {
                    tmp=matchs.next().captured(0);
                    tmp.remove(QRegularExpression("[\\s\\S]+ "));
                    t=tmp.toInt();
                    up+=t;
                }
               // qDebug()<<"got up-data"<<up<<"oup"<<oup;
               // QString text;

                t=up-oup;
                t=t/1024;
                if(t>999)
                {
                    t=t*100/1024;
                    float f=t*1.0/100;
                    upspeed="↑"+QString().setNum(f)+"M";

                }
                else if(t>0) upspeed+="↑"+QString().setNum(t)+"K";
                else upspeed="↑"+QString().setNum(up-oup)+"B";

                if(oup == 0) upspeed ="↑0B";//没有基数，网速不正确，设为0B。
                oup=up;

                reg.setPattern("mcast[\\s\\S]+?\\d+");
                matchs=reg.globalMatch(result);
                while(matchs.hasNext())
                {
                    tmp=matchs.next().captured(0);
                    tmp.remove(QRegularExpression("[\\s\\S]+ "));
                    t=tmp.toLongLong();
                    down+=t;
                   // qDebug()<<"got down"<<tmp;
                }
               // qDebug()<<"got down-data"<<down<<"odown"<<odown;
                t=down-odown;
                t=t/1024;
                if(t>999)
                {
                    t=t*100/1024;
                    float f=t*1.0/100;
                    downspeed="↓"+QString().setNum(f)+"M";

                }
                else if(t>0) downspeed="↓"+QString().setNum(t)+"K";
                else downspeed="↓"+QString().setNum(down-odown)+"B";

               // if(odown!=0) ui->down->setText(text);
                if(odown == 0) downspeed ="↓0B";//没有基数，网速不正确，设为0B。

                odown=down;
            }
        }
    }
}

void monitot::getmemused()
{

    QString result,tmp;
    pro->start("free -m");
    if(pro->waitForStarted(2000))
    {
        if(pro->waitForFinished(1000))
        {
            result=pro->readAllStandardOutput();
          //  qDebug()<<result;
            if(!result.isEmpty())
            {
                memused=0;
                qint64 total,used;
               // int omem=ui->mem->value();//先获取之前的数
                total=0;
                used=0;
                reg.setPattern("\\d+.+");
                //reg.setPattern("Mem:.+");
                tmp=reg.match(result).captured(0);
               // qDebug()<<tmp;
                QStringList list=tmp.split(QRegularExpression(" +"));
                result=list.at(0);//total
                tmp=list.at(5);//available
                total=result.toLongLong();
                used=tmp.toLongLong();
                memused=used*1000/total;
                memused=1000-memused;
                int m=memused%10;
                memused/=10;
                if(m>=5) memused+=1;//四舍五入
            }
        }
    }
}

void monitot::mousePressEvent(QMouseEvent* event)
{
    if(event->button()==Qt::LeftButton )
    {
        oldpos=event->globalPos()-this->pos();
        setCursor(Qt::ClosedHandCursor);
        justpress = true;
    }
}

void monitot::mouseMoveEvent(QMouseEvent * event){
    killer->stop_hide();
    move(event->globalPos()-oldpos);//貌似linux要这样
    event->accept();
    justpress = false;
}

void monitot::mouseReleaseEvent(QMouseEvent * event){
    setCursor(Qt::ArrowCursor);

    if(this->x()<=0) side = 0;
    else if(this->x()>=QApplication::desktop()->width()-86) side = 2;
    else if(this->y()<=0) side = 1;
    else side = -1;

    if(killer->isVisible()) {
        killer->stop_hide();
        return;
    }
    if(justpress)
    {
        int x,y;
        x=this->x();
        y=this->y()+32+4;
        if(x > QApplication::desktop()->width()-killer->width())  x = QApplication::desktop()->width()-killer->width();
        else if(x < 0) x=0;
        if(QApplication::desktop()->height()-this->y() < killer->height()){
            y = this->y()-killer->height()-4;
            qDebug()<<"QApplication::desktop()->height()-this->y() < killer->height()"<<y;
        }

        killer->move(x,y);
        killer->getinfo();
        killer->show();
        killer->activateWindow();
    }
    justpress=true;
    event->accept();
}

void monitot::enterEvent(QEvent *){
    showall =true;//立即开始画网速部分；
    int endx,endy;
    switch(side)
    {
    case 0:
    case 1:
        endx = this->x();
        endy = this->y();
        break;
    case 2:
        endx = this->x()-68;
        endy = this->y();
        break;
    default:
        return;
    }
    qDebug()<<endx<<endy;
    //QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(150);
    animation->setStartValue(QRect(this->x(), this->y(), 18, 32));

    animation->setEndValue(QRect(endx, endy, 86, 32));

    animation->start();
}
void monitot::leaveEvent(QEvent *){
    int endx,endy,endw,endh;
    endw = 18;
    endh = 32;
    switch(side)
    {
    case 0:
        endx = this->x();
        endy = this->y();
        break;
    case 1:
        endx = this->x();
        endy = this->y();
        endw = 32;
        endh = 18;
        break;
    case 2:
        endx = this->x()+68;
        endy = this->y();
        break;
    default:
        return;
    }

    animation->setDuration(150);
    animation->setStartValue(QRect(this->x(), this->y(), 86, 32));
    animation->setEndValue(QRect(endx,endy, endw, endh));
    qDebug()<<endx<<endy;

    animation->start();
}

void monitot::contextMenuEvent(QContextMenuEvent *) //右键菜单项编辑
{
QCursor cur=this->cursor();
QMenu *menu=new QMenu(this);
menu->addAction(moniter);//添加系统监视器
menu->addAction(tuichu); //添加退出
menu->exec(cur.pos()); //关联到光标
}

void monitot::openmoniter()
{
    QProcess process;
    process.startDetached("gnome-system-monitor");
}

void monitot::poscheck()
{
    int x=this->x();
    int y=this->y();
    //        qDebug()<<x<<endl<<y;

    if(this->pos().x()<0) x=0;
    else if(QApplication::desktop()->width()-x<this->width()){
      x=QApplication::desktop()->width()-this->width();
    }

    if(this->pos().y()<0) y=0;
    else if(QApplication::desktop()->height()-y<this->height())
    {
      y=QApplication::desktop()->height()-this->height();
    }
    //        qDebug()<<x<<endl<<y;
    move(x,y);
}

void monitot::animationfinished(){
    if(this->width()<86)  showall = false;
    else showall = true;
}
