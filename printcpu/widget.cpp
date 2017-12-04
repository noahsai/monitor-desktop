#include "widget.h"
#include "ui_widget.h"

//float a[31] = {0.0,0.3,0.0,0.201,0.10,0.609,0.1,0.40,1,0.4,0.0,0.3,0.0,0.201,0.10,0.609,0.1,0.40,1,0.4,0.0,0.3,0.0,0.201,0.10,0.609,0.1,0.40,1,0.4,0.2};
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    //this->setAttribute(Qt::WA_TranslucentBackground, true);
    howlong_fresh = 100;//多少毫秒刷新一次；
    show_second = 30;//显示框中头-尾总共show_second+1秒，因为1-show_second+1之间有10个间隔，10比较好算数
    times_1s = 1000/howlong_fresh;//1秒内几次
    time = 0;
    cpunum=0;
    // now  = 0;
    initlist();//放在show_second后面
//    timer.setSingleShot(false);
//    timer.setInterval(howlong_fresh);
//    connect(&timer ,SIGNAL(timeout()),this,SLOT(timeout()));
//    //adddata(a[now]);
//    timer.start();
    //resize(100,30);
    setMinimumSize(70,30);

}

Widget::~Widget()
{
    delete ui;
}


QSize Widget:: sizeHint()
{
    return QSize(100,30);
}



void Widget::initlist(){
    datalist.clear();
    for(int i =0;i<show_second+1;i++)
    {
        datalist.append(-1);
    }
}


void Widget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,false);
    p.setPen(QPen(QColor("#ddeeff"),1));
    for(int i=1;i<=5;i++){
        qreal pre = this->width()/6;
        p.drawLine(pre*i,0,pre*i,this->height());
    }
    for(int i =1;i<=3;i++){
        qreal pre = this->height()/4;
        p.drawLine(0,pre*i,this->width(),pre*i);
    }
    p.setRenderHint(QPainter::Antialiasing,true);
    QPainterPath path;
    bool start = false;
    qreal x=-1.0;
    qreal y=-1.0;
//    p.translate(0-this->width()/show_second,0);
    for (int i=0;i<show_second+1;i++)
    {
        if(datalist.at(i)==-1) continue;
        x = getx((i)*1000-howlong_fresh*time);
        y = gety(datalist.at(i));
        if((x!=-1.0) && start == false) {
            start = true;
            path.moveTo(x,this->height());
            path.lineTo(x,y);
        }
        else path.lineTo(x,y);
       // qDebug()<<"x,y"<<x<<y;
    }
    path.lineTo(x,this->height());
   // qDebug()<<path<<datalist;
    if(cpunum!=0)
    {
        p.setPen(QColor("#558cc8ff"));
        QFont fon;
        fon.setPixelSize(this->height()/3);
       // fon.setBold(true);
        p.setFont(fon);
        //p.drawText(this->rect(),QString().setNum(cpunum),QTextOption(Qt::AlignTop|Qt::AlignLeft));
        p.drawText(this->rect()," "+QString().setNum(cpunum));
    }
    //画曲线
    p.setBrush(QBrush(QColor("#99ddeeff")));
    p.setPen(QColor("#8cc8ff"));
    p.drawPath(path);
    //画边框
    p.setBrush(Qt::transparent);
    p.setPen(QPen(QColor("gray"),1));
    p.drawRect(this->rect());
    //写数字
    QFont fon;
    p.setPen(QColor("#222222"));
    int s = this->height()/3;
    if(s<10) s =10;
    fon.setPixelSize(s);
    p.setFont(fon);
    float n = datalist.last()*100;
    if(datalist.last()<=0) n = 0.0;
    p.drawText(this->rect(),QString().setNum(n,'f',1)+"%",QTextOption(Qt::AlignHCenter|Qt::AlignTop));
  //  qDebug()<<"paint";

}


//void Widget::timeout()
//{
//    time++;
//    if(time>=times_1s) {
//        if(now>show_second) {timer.stop();return;}
//        time =0;//放在后面可以在停止后保存time值，绘图就不会变
//        adddata(a[now]);
//        now ++;
//    }
//    update();

//}

void  Widget::adddata(float pre){
    datalist.takeFirst();
    datalist.append(pre);//写到此处
    if(isVisible()) update();
}

void  Widget::timechange(int time){
   this->time = time ;
    update();
}

void Widget:: set1s_times(int times){
    times_1s = times;
}


void Widget:: setcpunum(int n){
    cpunum = n;
}


qreal Widget::lms(int show_second , int times_1s){
    qreal w = this->width();
    qreal l = w/show_second/times_1s;// 宽度/10s/10次=100ms的长度
  //  qDebug()<<"lms"<<l;
    return l;
}

qreal Widget::getx( int ms){
    qreal x =  lms(show_second-1 ,times_1s) * (((qreal)(ms))/howlong_fresh) ;
    return x;
}

qreal Widget::gety(float per){
//    qreal y = (this->height()-20)*(1.0-per)+10;//上下边距10
    qreal y = (this->height())*(1.0-per);
 //   qDebug()<<"y"<<this->height()<<per;
    return y;
}
