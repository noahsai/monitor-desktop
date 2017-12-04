#include "manager.h"
#include "ui_manager.h"

manager::manager(QScrollArea *parent) :
    QScrollArea(parent),
    ui(new Ui::manager)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint
            |Qt::Tool
         );//去边框//最ding层显示//不在任务栏显示

    this->setAttribute(Qt::WA_TranslucentBackground, true);
    isone = true;
    row =0;
    col =0;
    time = 0;
    times_1s = 10;
    timer = new QTimer;
    initui();
    timer->setInterval(100);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeout()));
    timer->start();
    readset();
    setalpha( alpha );
  //  getcpu();
}

manager::~manager()
{
    saveset();
    delete ui;
}

void manager::getcpu()
{
    //qDebug()<<"getcpu start";
    QStringList* l;
    if(isone)
    {
        l=&list;
        isone = false;
    }
    else {
        l = &list2;
        isone = true;
    }
    l->clear();

    file.setFileName("/proc/stat");
    QTextStream in;
    in.setDevice(&file);
   if( file.open(QIODevice::ReadOnly))
   {
       QString data = in.readAll();
       //qDebug()<<data;
       reg.setPattern("cpu.+");
       matchs = reg.globalMatch(data);
       while(matchs.hasNext())
       {
           l->append(matchs.next().captured());
       }
       //qDebug()<<(*l);
       if(list.isEmpty()||list2.isEmpty()) {
           file.close();
           return;
       }
       else {
           result=getresult(list,list2);
       }
   }
   else {
       //qDebug()<<"file open error!";
   }
   file.close();
   QString info = QString().setNum( result.at(0)*100,'f',2)+"%";
   emit cpuchange(info);
   //qDebug()<<"getcpu ed";

}

void manager::timeout()
{
    timer->stop();
    time++;
    if(time>=times_1s) {
        time =0;//放在后面可以在停止后保存time值，绘图就不会变
        getcpu();
        if(!result.isEmpty()) refresh();
    }
    if(!result.isEmpty()) smallrefresh();
    timer->start();
}

 QList<float>   manager::getresult(QStringList& l1,QStringList&l2){
    QList<float> result;
    QString one,two;
    float res;
    for(int i=0;i<l1.count();i++)
    {
        one=l1.at(i);
        two=l2.at(i);
        res = colculate(one,two);
        result.append(res);
        //qDebug()<<"cpu"<<i<<":"<<res;
    }
//    qDebug()<<"result:"<<result;
    return result;
}

float  manager::colculate(QString& one,QString& two){
    float result;
    QStringList list;
    int sum1=0;
    int sum2=0;
    int idle1=0;
    int idle2=0;
    QList<int> intlist;
    list = one.split(QRegularExpression(" +"));
    for(int i=0;i<list.count();i++)
    {
        QString d=list.at(i);
        int dd=d.toInt();
        if(i==4) idle1=dd;
        intlist.append(dd);
        sum1+=dd;
    }
    list = two.split(QRegularExpression(" +"));
    for(int i=0;i<list.count();i++)
    {
        QString d=list.at(i);
        int dd=d.toInt();
        if(i==4) idle2=dd;
        intlist.append(dd);
        sum2+=dd;
    }
    result =1.0 - (idle2-idle1)*1.0/(sum2-sum1);

    return result;
}

void manager::initui(){
    //qDebug()<<"initui:";

    cpulist.clear();
    QFile file;
    file.setFileName("/proc/stat");
    QTextStream in;
    in.setDevice(&file);
   if( file.open(QIODevice::ReadOnly))
   {
       QString data = in.readAll();
       //qDebug()<<data;
       reg.setPattern("cpu.+");//包含总cpu
       matchs = reg.globalMatch(data);
       int i=0;//包含总cpu
       Widget* cpu;
       while(matchs.hasNext())
       {
           cpu = new Widget();
           cpu->setObjectName("cpumonitor");//给qss用的
           cpu->set1s_times(times_1s);
           cpu->setcpunum(i);
           cpulist.append(cpu);
           if(i==0)   ui->horizontalLayout->addWidget(cpu);
           //cpu->show();
           i++;
           matchs.next();
       }
       int n = i-1; //CPU数量
       //n=128;测试超多核
       if(n>8)
       {
          // int tmp =3;
           for(int i=(int)(qSqrt(n)+1);i>3;i--){
               if(n%i==0){
                   col = n/i;
                   row = n/col;
                   break;
               }
           }
           if(col == 0)
           {
               col = 3;
               row = n/col+1;
           }
       }
       else if(n>3){
            col =2;
            row =n/2;
            if(n%2!=0) n = n+1;
        }
        else {
            col = 1;
            row = n;
        }
        for(int i=0;i<row;i++){
            for(int j=0;j<col;j++){
                int n =i*(col)+j;
                //qDebug()<<i<<j<<n;
                cpu = cpulist.at(n+1);
                ui->gridLayout->addWidget(cpu,i,j);
            }
        }
        //qDebug()<<row<<col;
        //qDebug()<<"cpulist:"<<cpulist;
   }
   file.close();
   int tw,th;
   tw = 200-(col-1)*10;
   th = 50-(col-1)*2;
   if(tw<70) tw = 70;
   if(th<30) th = 30;

   resize(col*(4+tw),row*(4+th)*1.75);
}


void manager::refresh()
{
    //qDebug()<<"refresh start";
    Widget* cpu;
    cpulist.at(0)->adddata(result.at(0));//总的
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            int n =i*(col)+j;
            cpu = cpulist.at(n+1);//+1跳过总的，下同
            cpu->adddata(result.at(n+1));
        }
    }
    //qDebug()<<"refresh ed";

}

void manager::smallrefresh()
{
    //qDebug()<<"refresh start";
    Widget* cpu;
    cpu = cpulist.at(0);
    cpu->timechange(time);
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            int n =i*(col)+j;
            cpu = cpulist.at(n+1);
            cpu->timechange(time);
        }
    }
    //qDebug()<<"refresh ed";

}


void manager::mousePressEvent(QMouseEvent* event)
{
    if(event->button()==Qt::LeftButton )
    {
        oldpos=event->globalPos()-this->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    }
    else QWidget::mousePressEvent(event);
}

void manager::mouseMoveEvent(QMouseEvent * event){
    move(event->globalPos()-oldpos);//貌似linux要这样
    event->accept();
}

void manager::mouseReleaseEvent(QMouseEvent * event){
    if(event->button()==Qt::LeftButton )
    {
        setCursor(Qt::ArrowCursor);
        //先判断y
        if(this->y()<=0) {
            move(x(),0);
        }
        else if(this->y()>QApplication::desktop()->height()-this->height())
            move(x(),QApplication::desktop()->height()-this->height());
        //后判断x，这样就左右优先，上其次
        if(this->x()<=0) {
            move(0,y());
        }
        else if(this->x()>=QApplication::desktop()->width()-this->width()) {
            move(QApplication::desktop()->width()-this->width(),y());
        }
        saveset();
        event->accept();

    }
    else QWidget::mouseReleaseEvent(event);
}

void manager::saveset()
{
    QSettings set("Noahsai","Cpu-Monitor",this);
    set.setValue("geometry", this->geometry());
    set.setValue("visible", this->isVisible());
    set.setValue("alpha", alpha);
    //qDebug()<<alpha;


}
void manager::readset()
{
    QSettings set("Noahsai","Cpu-Monitor",this);

    QRect r = set.value("geometry",QVariant(QRect(QApplication::desktop()->width()-this->width(),QApplication::desktop()->height()-this->height() , this->width() , this->height()))).toRect();
    this->setGeometry(r);
    this->setVisible(set.value("visible",true).toBool());
    alpha = set.value("alpha" , 230).toInt();
    //qDebug()<<alpha;
}

void manager::mouseDoubleClickEvent(QMouseEvent * event){
    if(event->button()==Qt::LeftButton )
    {
     //   qDebug()<<windowFlags();
        if(windowFlags()==(Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowCloseButtonHint))
        {
            setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
            this->show();
        }
        else {
            setWindowFlags((Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowCloseButtonHint));
            this->showNormal();
        }

        event->accept();

    }
    else QWidget::mouseReleaseEvent(event);
}
void manager::closeEvent(QCloseEvent *event){
    hide();
    event->accept();
}

void manager::wheelEvent(QWheelEvent *event){
    if(QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        QWheelEvent *e = static_cast<QWheelEvent*>(event);
        int degrees = e->delta();//上为正，下为负
        if(degrees > 0)
        {
            alpha +=10;
            if(alpha > 255) alpha = 255;
        }
        else{
            alpha -=10;
            if(alpha < 0) alpha = 0;
        }
        setalpha(alpha);
        event->accept();
    }
    else QWidget::wheelEvent(event);
}

void manager::setalpha(int a)
{
    //qDebug()<<a;
    setStyleSheet("#manager,#scrollAreaWidgetContents,#cpumonitor{\
                  background-color:rgba(255, 255, 255, " + QString().setNum(a) + ");}");
}
