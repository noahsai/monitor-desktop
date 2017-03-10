#include "monitorwindow.h"
#include "ui_monitorwindow.h"
#include<monitot.h>

monitorwindow::monitorwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::monitorwindow)
{
    ui->setupUi(this);
    this->setGeometry(QApplication::desktop()->width()-120,QApplication::desktop()->height()-120,86,32);
    setWindowFlags(Qt::FramelessWindowHint
           |Qt::WindowStaysOnTopHint
            |Qt::Tool
         );//去边框//最ding层显示//不在任务栏显示
    ui->tableWidget->setColumnCount(4);

//    QStringList header;
//     header<<tr("进程")<<tr("CPU%")<<tr("内存%")<<tr(" ");
//    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->resizeSection(0,120);
    ui->tableWidget->horizontalHeader()->resizeSection(1,40);
    ui->tableWidget->horizontalHeader()->resizeSection(2,40);
    ui->tableWidget->horizontalHeader()->resizeSection(3,30);
    ui->tableWidget->horizontalHeader()->setFixedHeight(20);
    ui->tableWidget->verticalHeader() ->setDefaultSectionSize(25);
    ui->tableWidget->setFrameShape(QFrame::NoFrame);
    ui->tableWidget->setFixedSize(230,250);

    adjustSize();
    setFixedSize(QSize(230,320));//10+30+10+20+250
    cmd = MEM_SORT;
    process = new QProcess(this);
    timer = new QTimer(this);
    timer->setInterval(3000);
    timer->setSingleShot(true);//手动开始/停止
    connect(timer,SIGNAL(timeout()),this,SLOT(getinfo()));
    //timer->start();//
    getinfo();//timer->start()在里面
}

monitorwindow::~monitorwindow()
{
    delete ui;
}

void monitorwindow::stop_hide()
{
    timer->stop();
    hide();
}

bool monitorwindow::getinfo()
{
    timer->stop();

    listupdate2freshselected.clear();
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    QRegularExpression reg;
    QStringList list;
    QTableWidgetItem *item;

    monitot *m = (monitot*)(this->parentWidget());
    int memused = m->memusing();
    QString color=BG_color;
    if(memused>=80)//内存大时的颜色！默认红色
    {
        color = color.arg(HIGH).arg(HIGH);
    }
     else if(memused>=65&&memused<80)//内存占用较多时的颜色！默认橙色
     {
        color = color.arg(MID).arg(MID);
     }
     else   color = color.arg(LOW).arg(LOW);
    this->setStyleSheet(color);
    //========================
    process->start("/bin/bash");
    ui->tableWidget->clear();
    if(process->waitForStarted(3000))
    {
        process->write(cmd.toLatin1());
        process->closeWriteChannel();
        process->waitForFinished(3000);
        QString result = process->readAllStandardOutput();
        reg.setPattern(" +");
        result = result.replace(reg," ");
        //qDebug()<<result.split("\n");
        list = result.split("\n");
       // qDebug()<<list;
        list.removeFirst();//去除表头
        list.removeLast();//去除最后换行
        ui->tableWidget->setRowCount(list.length());
        for(int i=0;i<list.length();i++)
        {

            QString line,pid;
            line = list.at(i);
            QStringList one = line.split(" ");
            pid = one.at(1);
         //   qDebug()<<one;
            bool root=false;
            for(int j=0;j<ui->tableWidget->columnCount();j++)
            {

                item = new QTableWidgetItem;
                if(i%2!=0)item->setBackgroundColor(QColor("#f5f5f5"));
                else item->setBackgroundColor(QColor("#ffffff"));
                if(j==0)
                {
                    item->setWhatsThis(pid);
                    line = one.at(4);
                    if((QString(one.at(0))=="root"))    {
                        root = true;

                        line ="[Root] "+line;
                    }
                    item->setText(line);
                    line = one.mid(5).join(" ");
                    item->setToolTip(line);

                }
                else if(j==1)
                {
                    item->setTextAlignment(Qt::AlignHCenter	);
                    item->setText(one.at(2));
                }
                else if(j==2)
                {
                    line = one.at(3);
                    item->setTextAlignment(Qt::AlignHCenter	);
                    item->setText(line);
                }
                else if(j==3)
                {
                    if(!root)
                    {
                        item->setCheckState(Qt::Unchecked);
                        if(pidselected(pid)) {
                            //item->setSelected(true);
                            item->setCheckState(Qt::Checked);
                        }
                    }
                }
                ui->tableWidget->setItem(i,j,item);
                if(pid==pressed) ui->tableWidget->selectRow(i);
            }
        }
        //listupdate2freshselected 是刷新后的选择，selected应清空再赋值
        //主要是防止cpu和内存切换后，已选择的进程不在列表中了，误删看不见但在selected的进程
        selected.clear();
        selected = listupdate2freshselected;
        listupdate2freshselected.clear();

    }
    timer->start();
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
}



void monitorwindow::on_pushButton_3_clicked()
{
    listupdate2freshselected.clear();
    if(cmd == MEM_SORT)  return;
    cmd = MEM_SORT;
    getinfo();
}

void monitorwindow::on_pushButton_2_clicked()
{
    listupdate2freshselected.clear();
    if(cmd == CPU_SORT)  return;
    cmd = CPU_SORT;
    getinfo();
}

void monitorwindow::on_tableWidget_itemSelectionChanged()
{
    pressed = ui->tableWidget->item(ui->tableWidget->currentRow(),0)->whatsThis();
   // qDebug()<<"clicked pid"<<pressed;
//    if(!timer->isActive()) return;
//    selected.clear();
//    listupdate2freshselected.clear();
//    qDebug()<<"selectionchaged";
//    QList<QTableWidgetItem *>  list = ui->tableWidget->selectedItems();
//    for(int i=0;i<list.length();i++)
//    {
//        if(list.at(i)->column()==0)
//        {
//            selected.append(list.at(i)->whatsThis());
//        }
//    }
//    qDebug()<<selected<<selected.length();
}

bool monitorwindow::pidselected(const QString &pid)
{
    QString sel;
    for(int i=0;i<selected.length();i++)
    {
        sel=selected.at(i);
        if(pid==sel) {
            listupdate2freshselected.append(sel);
            return true;
        }
    }
    return false;
}

void monitorwindow::on_pushButton_4_clicked()
{
    timer->stop();
    this->hide();
}

void monitorwindow::on_pushButton_clicked()
{
    if(selected.isEmpty()) return;
    ui->pushButton->setEnabled(false);
    QProcess kill;
    QString cmd;
    cmd = "kill "+selected.join(" ");
    kill.start(cmd);
    if(kill.waitForStarted(3000))
        if(kill.waitForFinished(3000))
        {
            qDebug()<<cmd<<selected.length()<<" program killed.";
        }
    qDebug()<<kill.readAll();
    selected.clear();
    ui->pushButton->setEnabled(true);
}



void monitorwindow::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    if(item->column()!=3) return;
    //qDebug()<<item->checkState();
    if(item->checkState()==Qt::Checked)
    {
        selected.append(ui->tableWidget->item(item->row(),0)->whatsThis());
    }
    else if(item->checkState()==Qt::Unchecked) selected.removeOne(ui->tableWidget->item(item->row(),0)->whatsThis());
    qDebug()<<selected;
}

void monitorwindow::mousePressEvent(QMouseEvent* event)
{
    if(event->button()==Qt::LeftButton )
    {
        oldpos=event->globalPos()-this->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void monitorwindow::mouseMoveEvent(QMouseEvent * event){
    move(event->globalPos()-oldpos);//貌似linux要这样
    event->accept();
}

void monitorwindow::mouseReleaseEvent(QMouseEvent * event){
    setCursor(Qt::ArrowCursor);
    event->accept();
}
