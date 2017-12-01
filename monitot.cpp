#include "monitot.h"
#include "ui_monitot.h"
#include "monitorwindow.h"
#define  BG_W  90
// 2padding + 1反锯齿x2 + 86内容
#define  BG_H  36
// 2padding + 1反锯齿x2 + 32内容
#define  BG_PADD 2
//padding
#define  MEM_S  32
//内存球绘图大小 = 1反锯齿x2 + 30内容

monitot::monitot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::monitot)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint
       |Qt::WindowStaysOnTopHint
        |Qt::Tool
     );//去边框//最ding层显示//不在任务栏显示
    setAutoFillBackground(false);
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    menu =new QMenu(this);
    QAction *tuichu  = new QAction("退出",menu);
    QAction *moniter = new QAction("系统监视器",menu);
    QAction *cpu = new QAction("CUP图开/关",menu);
    connect(tuichu, SIGNAL(triggered(bool)), qApp, SLOT(quit()));//若触发了退出就退出程序
    connect(moniter,SIGNAL(triggered(bool)),this,SLOT(openmoniter()));
    connect(cpu,SIGNAL(triggered(bool)),this,SLOT(opencpu()));
    menu->addAction(cpu); //添加退出
    menu->addAction(moniter);//添加系统监视器
    menu->addAction(tuichu); //添加退出

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
    cpumonitor = new manager();
    connect(cpumonitor, SIGNAL(cpuchange(QString&)),killer,SLOT(cpuchange(QString&)));
    cpumonitor->hide();
    timer = new QTimer(this);
    timer->setInterval(1000);
    timer->setSingleShot(false);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeout()));
    timer->start();
    readset();
    poscheck();
}

monitot::~monitot()
{
    saveset();
    delete ui;
}

void monitot::paintEvent(QPaintEvent *event)
{
    QFont font;
    QPainter paint(this);
    paint.setRenderHint(QPainter::Antialiasing,true);
    //===============先画底============
    QLinearGradient linearGradient(0,0,0,BG_H);
    //创建了一个QLinearGradient对象实例，参数为起点和终点坐标，可作为颜色渐变的方向
    //painter.setPen(Qt::NoPen);
    linearGradient.setColorAt(0.0,Qt::white);
    linearGradient.setColorAt(1.0,QColor(224, 224, 224));
    paint.setBrush(QBrush(linearGradient));
    paint.setPen(QPen(QColor(210, 210, 210), 1));

    if(side==0&&(!showall))                    paint.drawRoundedRect(0-BG_H/2+1 , 1 , BG_H - 2  , BG_H - 2 , BG_H/2,BG_H/2);//背景圆球
    else if(side == 1&&(!showall)) {
        //paint.translate(0,0-BG_H/2-1);
        paint.drawRoundedRect(1,0-BG_H/2+1,BG_H - 2  , BG_H - 2 , BG_H/2,BG_H/2);//背景圆球
    }
    else
    {
        //  paint.setBrush(QBrush(Qt::white));//↓由于使用了反锯齿，四边都要多1像素，需要右下各移动1像素再绘图，确保边缘平滑显示。
        paint.drawRoundedRect(1,1,BG_W - 2  , BG_H - 2 , BG_H/2,BG_H/2);
    }
    if(showall)
    {
        //=========================再画网速数值==================
        font.setPixelSize(12);
        paint.setFont(font);
        paint.setPen(QPen(Qt::red, 1));
        paint.drawText(QRectF(MEM_S ,BG_H/2-14,58,14),Qt::AlignLeft|Qt::AlignVCenter,upspeed);

        paint.setPen(QPen(QColor(60, 150, 21), 1));
        paint.drawText(QRectF(MEM_S,BG_H/2,58,14),Qt::AlignLeft|Qt::AlignVCenter,downspeed);
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
    paint.save();//保持(0,0)原点
    paint.translate(0, BG_PADD+1);//改变纵坐标原点，使填图案时不会发生错误 //由于反锯齿，还要+1
    paint.setBrush(QBrush(Qt::green,pix));
    paint.setPen(QPen(QColor(210, 210, 210), 1));
    if(side==0&&(!showall)) {
        paint.drawRoundedRect(0-BG_H/2+BG_PADD+1 , 0 ,30,30,15,15);//由于移坐标时y已经+1，此处y就不用+1了
    }
    else if(side == 1&&(!showall)) {
        paint.translate(0, 0-BG_H/2);//改变纵坐标原点，使填图案时不会发生错误
        paint.drawRoundedRect(BG_PADD+1 ,0 ,30,30,15,15);//由于移坐标时y已经+1，此处y就不用+1了
    }
    else paint.drawRoundedRect(BG_PADD+1 , 0 ,30,30,15,15);//由于移坐标时y已经+1，此处y就不用+1了
    paint.restore();//返回saved的0,0
    //==================再写内存数值==============================
    if(showall)
    {
        paint.setPen(QPen(QColor(BG_H/20, BG_H/20, BG_H/20), 1));
        font.setPixelSize(16);
        paint.setFont(font);
        paint.drawText(QRectF(BG_PADD+1,1,30,BG_H-4),Qt::AlignCenter,QString().setNum( memused));
        paint.setPen(QPen(QColor(255, 255, 255), 1));
        paint.drawText(QRectF(BG_PADD+0,1,30,BG_H-4),Qt::AlignCenter,QString().setNum( memused));
    }
    else {
        paint.setPen(QPen(QColor(150, 150, 150), 1));
        font.setPixelSize(12);
        paint.setFont(font);
         if(side ==0)
        {
            paint.drawText(QRectF(0 , 0,MEM_S/2,BG_H),Qt::AlignCenter,QString().setNum( memused));
            paint.setPen(QPen(QColor(255, 255, 255), 1));
            paint.drawText(QRectF(-1 , 0,MEM_S/2,BG_H),Qt::AlignCenter,QString().setNum( memused));
        }
        else if(side ==1)
        {
             paint.drawText(QRectF(0,0, BG_H , MEM_S/2),Qt::AlignCenter,QString().setNum( memused));
             paint.setPen(QPen(QColor(255, 255, 255), 1));
             paint.drawText(QRectF(0, -1 ,BG_H , MEM_S/2),Qt::AlignCenter,QString().setNum( memused));
        }
        else
        {
            paint.drawText(QRectF(BG_PADD+2,0,MEM_S/2,BG_H),Qt::AlignCenter,QString().setNum( memused));
            paint.setPen(QPen(QColor(255, 255, 255), 1));
            paint.drawText(QRectF(BG_PADD+1,0,MEM_S/2,BG_H),Qt::AlignCenter,QString().setNum( memused));
        }
    }
    //QWidget::paintEvent(event);
    event->accept();
}


void monitot::timeout()
{
    if(killer->isVisible()&&(!killer->isActiveWindow()))
    {
        killer->stop_hide();
    }
   if(showall) {
       getnetspeed();
   }
   else {
       oup = 0;
       odown = 0;
   }
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
        event->accept();
    }
    else QWidget::mousePressEvent(event);
}

void monitot::mouseMoveEvent(QMouseEvent * event){
    killer->stop_hide();
    move(event->globalPos()-oldpos);//貌似linux要这样
    justpress = false;
    event->accept();
}

void monitot::mouseReleaseEvent(QMouseEvent * event){
    if(event->button()==Qt::LeftButton )
    {
        setCursor(Qt::ArrowCursor);
        side = -1;//默认为不靠边
        //先判断y
        if(this->y()<=0) {
            side = 1;
            move(x(),0);
        }
        //后判断x，这样就左右优先，上其次
        if(this->x()<=0) {
            side = 0;
            move(0,y());
        }
        else if(this->x()>=QApplication::desktop()->width()-BG_W) {
            move(QApplication::desktop()->width()-BG_W,y());
            side = 2;
        }

        if(killer->isVisible()) {
            killer->stop_hide();
            return;
        }
        if(justpress)
        {
            int x,y;
            x=this->x();
            y=this->y()+BG_H+4;
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
      //  qDebug()<<x()<<y();
        justpress=true;
        saveset();
        event->accept();

    }
    else QWidget::mouseReleaseEvent(event);
}

void monitot::enterEvent(QEvent *event){
    if(animation->state()==QAbstractAnimation::Running) {
        qDebug()<<"animation running";
        return;
    }
    showall =true;//立即开始画网速部分；
    int endx,endy,startw,starth;//
    startw = BG_H/2;
    starth = BG_H;
    switch(side)
    {
    case 0:
        endx = this->x();
        endy = this->y();
        break;
    case 1:
        endx = this->x();
        endy = this->y();
        startw = BG_H;
        starth = BG_H/2;
        break;
    case 2:
        endx = this->x()-(BG_W-BG_H/2);
        endy = this->y();
        break;
    default:
        return;
    }
    qDebug()<<endx<<endy<<startw<<starth;
    //QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->stop();
    animation->setDuration(150);
    animation->setStartValue(QRect(this->x(), this->y(), startw, starth));

    animation->setEndValue(QRect(endx, endy, BG_W, BG_H));
   qDebug()<<"enter"<<this->geometry();

    animation->start();
    event->accept();
}
void monitot::leaveEvent(QEvent *event){
    int endx,endy,endw,endh;
    endw = BG_H/2;
    endh = BG_H;
    showall =true;
    switch(side)
    {
    case 0:
        endx = this->x();
        endy = this->y();
        break;
    case 1:
        endx = this->x();
        endy = this->y();
        endw = BG_H;
        endh = BG_H/2;
        break;
    case 2:
        endx = this->x()+(BG_W-BG_H/2);
        endy = this->y();
        break;
    default:
        return;
    }
    animation->stop();
    animation->setDuration(150);
    animation->setStartValue(QRect(this->x(), this->y(), this->width(), this->height()));
    animation->setEndValue(QRect(endx,endy, endw, endh));
    qDebug()<<endx<<endy<<endw<<endh;
    qDebug()<<"leave"<<this->geometry();
    animation->start();
    event->accept();
}

void monitot::contextMenuEvent(QContextMenuEvent * event) //右键菜单项编辑
{
    if(menu)
    {
        QCursor cur=this->cursor();
        menu->exec(cur.pos()); //关联到光标

    }
    event->accept();
}

void monitot::openmoniter()
{
    QProcess process;
    process.startDetached("gnome-system-monitor");
}

void monitot::opencpu()
{
    cpumonitor->setVisible(!cpumonitor->isVisible());
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
    if(this->width()<BG_W)  showall = false;
    else showall = true;
   // qDebug()<<"animationfinished"<<showall;
}

void monitot::saveset()
{
    QSettings set("Noahsai","Monitor-desktop",this);
    set.setValue("geometry", this->geometry());
    set.setValue("side", side);
    set.setValue("showall",showall);
}
void monitot::readset()
{
    QSettings set("Noahsai","Monitor-desktop",this);
    QRect r = set.value("geometry",QVariant(QRect(QApplication::desktop()->width()-100,QApplication::desktop()->height()-100 , BG_W , BG_H))).toRect();
    int rx = r.x();
    int ry = r.y();
    setGeometry(rx , ry , BG_W , BG_H );
    side = set.value("side", -1).toInt();
    showall = set.value("showall",true).toBool();
}

int monitot::memusing(){
    return memused;
}
