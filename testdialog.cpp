#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include <QMessageBox>
#include "testdialog.h"
#include "ui_testdialog.h"
#include <QDebug>
#include <QFile>
#include <QProcess>
#include "touchtest.h"
#include "lcdtestdialog.h"

#include<stdio.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "myapplication.h"
#include "calculator.h"
#include "mylineedit.h"
#include "infomationdialog.h"
#include "questiondialog.h"
#include "monitorudp.h"

#define AI_DATA_OFFSET 192
//#define AI_DATA_OFFSET 80
#define TEST_DATA_OFFSET 112

#define I2C_ADDR 0x57
#define AI_I2C_ADDR 0x53
#define FRAM_ADDR 0x53

#ifdef __linux__
#include <sys/ioctl.h>
#include <termios.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/reboot.h>
#include <linux/rtc.h>
#endif

int fd;
int afd;
quint8 doValue;
quint8 diBuf[4];
extern bool        m_bScreenSave;
extern double AdcCalibraion[6][2];
extern QString gSoftwareModel;
extern QString gHardwareModel;
const char *ComStr[12] = {
    "/dev/ttyS1",
    "/dev/ttyS3",
    "/dev/ttyS5",
    "/dev/ttyS7",
    "/dev/ttyUSB0",
    "/dev/ttyUSB1",
    "/dev/ttyUSB2",
    "/dev/ttyUSB3",
    "/dev/ttyUSB4",
    "/dev/ttyUSB5",
    "/dev/ttyUSB6",
    "/dev/ttyUSB7",
};

extern bool    m_bScreenSave;

CTestDialog::CTestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CTestDialog)
{
    int flag = O_RDWR;
#ifdef __linux__
    flag |= O_NDELAY;
#endif
    fd = ::open("/dev/iostardust", flag);
    afd = ::open("/dev/sun_ai", flag);
    if (fd < 0) {
        printf("ERROR: /dev/sun open(%d) failed\n", fd);
    }
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    memcpy(m_AdcCalibraion, AdcCalibraion, sizeof(AdcCalibraion));

    redPalette.setColor(QPalette::Button, Qt::red);
    greenPalette.setColor(QPalette::Button, Qt::green);

    ui->do1->setPalette(greenPalette);

    m_diButton[0] = ui->di1;
    m_diButton[1] = ui->di2;
    m_diButton[2] = ui->di3;
    m_diButton[3] = ui->di4;
    m_diButton[4] = ui->di5;
    m_diButton[5] = ui->di6;
    m_diButton[6] = ui->di7;
    m_diButton[7] = ui->di8;
    m_diButton[8] = ui->di9;
    m_diButton[9] = ui->di10;
    m_diButton[10] = ui->di11;
    m_diButton[11] = ui->di12;
    m_diButton[12] = ui->di13;
    m_diButton[13] = ui->di14;
    m_diButton[14] = ui->di15;
    m_diButton[15] = ui->di16;

    m_doButton[0] = ui->do1;
    m_doButton[1] = ui->do2;
    m_doButton[2] = ui->do3;
    m_doButton[3] = ui->do4;
    m_doButton[4] = ui->do5;
    m_doButton[5] = ui->do6;
    m_doButton[6] = ui->do7;
    m_doButton[7] = ui->do8;

    m_aiLcd[0] = ui->ai1;
    m_aiLcd[1] = ui->ai2;
    m_aiLcd[2] = ui->ai3;
    m_aiLcd[3] = ui->ai4;
    m_aiLcd[4] = ui->ai5;
    m_aiLcd[5] = ui->ai6;
    m_ai4Lcd[0] = ui->ai1_21;
    m_ai4Lcd[1] = ui->ai2_21;
    m_ai4Lcd[2] = ui->ai3_21;
    m_ai4Lcd[3] = ui->ai4_21;
    m_ai4Lcd[4] = ui->ai5_21;
    m_ai4Lcd[5] = ui->ai6_21;

    m_busbhub1 = m_busbhub2 = m_busbhub3 = true;
    ui->usbHub1->setPalette(redPalette);
    ui->usbHub2->setPalette(redPalette);

    m_timerId = startTimer(100);
    m_timerIdAi = startTimer(100);

#ifdef __linux__
       ioctl(fd, IOCTL_LCD_ONOFF, 1);
#endif

#ifdef __linux__
        ioctl(fd, IOCTL_DOUT_READ, &doValue);
#endif
    doDisp(doValue);
    m_timerIdCom1 = m_timerIdCom2 = m_timerIdCom3 =
    m_timerIdCom4Com5 = m_timerIdCom5Com4 = m_timerIdUsbSerial =
    m_timerIdPing = m_timerIdPing2 = m_timerIdCommon = 0;

    setText(ui->ai1_4, 1, 0);
    setText(ui->ai2_4, 2, 0);
    setText(ui->ai3_4, 3, 0);
    setText(ui->ai4_4, 4, 0);
    setText(ui->ai5_4, 5, 0);
    setText(ui->ai6_4, 6, 0);

    setText(ui->ai1_20, 1, 1);
    setText(ui->ai2_20, 2, 1);
    setText(ui->ai3_20, 3, 1);
    setText(ui->ai4_20, 4, 1);
    setText(ui->ai5_20, 5, 1);
    setText(ui->ai6_20, 6, 1);

#ifdef __linux__
//    char buf[50];
//    ::read(m_Fd4, buf, 1);

   if (( mRtc = ::open ( "/dev/rtc0", O_RDWR)) < 0 ) {
       CInfomationDialog dlg("RTC Open Error");
       dlg.exec();
   }
#endif
   QWidget *widget;
   mDiMax = 7;
#ifdef __linux__
   for(int i = 0; i < 12; i++)
   {
       m_Fd[i] = ::open(ComStr[i], O_RDWR | O_NOCTTY |O_NONBLOCK );
       setupComm(m_Fd[i]);
   }
#endif
   widget = ui->stackedWidget->widget(2);
   ui->stackedWidget->removeWidget(widget);
   widget = ui->stackedWidget->widget(0);
   ui->stackedWidget->removeWidget(widget);
   on_CommClear_clicked();
   m_diButton[0] = ui->ss1000di_1;
   m_diButton[1] = ui->ss1000di_2;
   m_diButton[2] = ui->ss1000di_3;
   m_diButton[3] = ui->ss1000di_4;
   m_diButton[4] = ui->ss1000di_5;
   m_diButton[5] = ui->ss1000di_6;
   m_diButton[6] = ui->ss1000di_7;

   ui->stackedWidget->setCurrentIndex(0);
   ui->ioTest->setChecked(true);
   ui->serialTest->setChecked(false);
   ui->etcTest->setChecked(false);

   if(QFile("/tmp/__RTC_LOWBATTERY__").exists() == false)
       ui->labelLowBattery->setVisible(false);
   connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
   m_Timer.start(1000);
}

CTestDialog::~CTestDialog()
{
    delete ui;
}

void CTestDialog::paintEvent(QPaintEvent *)
{
    QPainter	p(this);
    QPen pen;
    pen.setWidth(3);
    pen.setColor(Qt::white);

    QSize size = frameSize();
    p.setPen(pen);
    p.drawLine(0, 0, size.rwidth(), 0);
    p.drawLine(0, 0, 0, size.rheight());
    pen.setColor(Qt::darkGray);
    p.setPen(pen);
    p.drawLine(size.rwidth(), 0, size.rwidth(), size.rheight());
    p.drawLine(0, size.rheight(), size.rwidth(), size.rheight());
}

void CTestDialog::msgPut(const QString& str)
{
    ui->listWidgetMsg->addItem(str);
}

void CTestDialog::on_pushButtonClear_clicked()
{
   ui->listWidgetMsg->clear();
}

void CTestDialog::doDisp(int value)
{
    for(int i = 0; i < 8; i++)
    {
        qint8 mask = 1 << i;
        if(value & mask)
            m_doButton[i]->setPalette(redPalette);
        else
            m_doButton[i]->setPalette(greenPalette);
    }
}

void CTestDialog::diDisp(int value)
{
    int mask;
    for(int i = 0; i < mDiMax; i++)
    {
        mask = 0x01 << i;
        if(value & mask)
            m_diButton[i]->setPalette(redPalette);
        else
            m_diButton[i]->setPalette(greenPalette);
    }
}

void CTestDialog::aiDisp()
{

    QFile file("/sys/class/hwmon/hwmon0/in_all_raw");

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList ai = line.split(',',QString::SkipEmptyParts);
            if (ai.count() >= 6) {
                for(int i = 0; i < 6; i++)
                    m_Value[i] = ai[i].toDouble();
            }
            break;
        }
        file.close();
    }
    // 현재의 io상태 읽기
    for(int i = 0; i < 6 ;++i)
    {
        m_aiLcd[i]->display( (double)m_Value[i]);
        double val = 0;
        if(m_AdcCalibraion [i][0] < m_AdcCalibraion [i][1])
            val = ((m_Value[i] - m_AdcCalibraion [i][0]) * 16.0) / (double)(m_AdcCalibraion [i][1] - m_AdcCalibraion [i][0]) + 4;
        m_ai4Lcd[i]->display(QString("%1").arg(val, 0, 'f', 2));
/*        qDebug() << "AI" << i << " L:" << m_AdcCalibraion [i][0] << " H:" << m_AdcCalibraion [i][1] << " CURR:" << m_Value[i] << " Calc:" << val;
        qDebug() << "c1:" << ((m_Value[i] - m_AdcCalibraion [i][0]) * 16.0);
        qDebug() << "c2:" << (m_AdcCalibraion [i][1] - m_AdcCalibraion [i][0]);
        */
    }
}
void CTestDialog::timerEvent(QTimerEvent *event)
{
    QString send;
#ifdef __linux__
    if (event->timerId() == m_timerId)
    {
        read(fd, diBuf, 4);
        int val = diBuf[0] | diBuf[1] << 8;

        diDisp(val);
        tm rtc_tm;
        int now;
        /* Read the RTC time/date */
        int retval = ioctl(mRtc, RTC_RD_TIME, &rtc_tm);
        if (retval == -1) {
            perror("RTC_RD_TIME ioctl");
        }
        now = mktime(&rtc_tm);
        QDateTime time;
        time = QDateTime::fromTime_t(now);
        ui->labelRTC->setText(QString(time.toString("yyyy:MM:dd hh:mm:ss")));
    }
    else
    if (event->timerId() == m_timerIdCom1)
    {
        send = ui->com1Tx->text();
        readComm(m_Fd1, send, m_timerIdCom1, ui->com1Rx);
    }
    else
    if (event->timerId() == m_timerIdCom2)
    {
        send = ui->com2Tx->text();
        readComm(m_Fd2, send, m_timerIdCom2, ui->com2Rx);
    }
    else
    if (event->timerId() == m_timerIdCom3)
    {
        send = ui->com3Tx->text();
        readComm(m_Fd3, send, m_timerIdCom3, ui->com3Rx);
    }
    else
    if (event->timerId() == m_timerIdCom4Com5)
    {
        send = ui->com4Tx->text();
        readComm(m_Fd5, send, m_timerIdCom4Com5, ui->com5Rx);
    }
    else
    if (event->timerId() == m_timerIdCom5Com4)
    {
        send = ui->com5Tx->text();
        readComm(m_Fd4, send, m_timerIdCom5Com4, ui->com4Rx);
    }
    else
    if (event->timerId() == m_timerIdCommon)
    {
        send = ui->commTx->text();
        readComm(m_Fd[CommRxSeleced], send, m_timerIdCommon, ui->commRx);
    }
    else
    if (event->timerId() == m_timerIdUsbSerial)
    {
        killTimer(m_timerIdUsbSerial);
        QFile file("/tmp/setusb.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray ar = file.readAll();
        ui->listWidgetMsg->addItem(QString(ar.data()));
    }
    else
#endif
    if (event->timerId() == m_timerIdAi)
    {
        aiDisp();
    }
    else
    if (event->timerId() == m_timerIdPing)
    {
        killTimer(m_timerIdPing);
        QFile file("/tmp/ping.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray ar;
        for(int i = 0; i < 2; i++)
            ar = file.readLine();
        ar = file.readAll();
        ui->listWidgetMsg->addItem(QString(ar.data()));
    }
    else
    if (event->timerId() == m_timerIdPing2)
    {
        killTimer(m_timerIdPing2);
        QFile file("/tmp/ping2.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray ar;
        for(int i = 0; i < 2; i++)
            ar = file.readLine();
        ar = file.readAll();
        ui->listWidgetMsg->addItem(QString(ar.data()));
    }
}

quint8 buf[10];

void CTestDialog::doSetValue(int pos)
{
#ifdef __linux__
    qint8 mask = 1 << pos;
    if(doValue & mask)
    {
        m_doButton[pos]->setPalette(greenPalette);
        doValue = doValue & ~mask;
    }
    else
    {
        m_doButton[pos]->setPalette(redPalette);
        doValue = doValue | mask;
    }
    buf[0] = doValue;
    write(fd, buf, 1);
#endif
}

void CTestDialog::on_do1_clicked()
{
    doSetValue(0);
}

void CTestDialog::on_do2_clicked()
{
    doSetValue(1);
}

void CTestDialog::on_do3_clicked()
{
    doSetValue(2);
}

void CTestDialog::on_do4_clicked()
{
    doSetValue(3);
}

void CTestDialog::on_do5_clicked()
{
    doSetValue(4);
}

void CTestDialog::on_do6_clicked()
{
    doSetValue(5);
}

void CTestDialog::on_do7_clicked()
{
    doSetValue(6);
}

void CTestDialog::on_do8_clicked()
{
    doSetValue(7);
}

void CTestDialog::on_lcdOff_clicked()
{
    m_bScreenSave = true;
#ifdef __linux__
       ioctl(fd, IOCTL_LCD_ONOFF, 0);
#endif
}

void CTestDialog::on_usbHub1_clicked()
{
    if(m_busbhub1 == true)
    {
        m_busbhub1 = false;
        ui->usbHub1->setPalette(greenPalette);
#ifdef __linux__
        ioctl(fd, IOCTL_USB2_ONOFF, 0);
#endif
    }
    else
    {
        m_busbhub1 = true;
        ui->usbHub1->setPalette(redPalette);
#ifdef __linux__
        ioctl(fd, IOCTL_USB2_ONOFF, 1);
#endif
    }
}

void CTestDialog::on_usbHub2_clicked()
{
    if(m_busbhub2 == true)
    {
        m_busbhub2 = false;
        ui->usbHub2->setPalette(greenPalette);
#ifdef __linux__
        ioctl(fd, IOCTL_USB1_ONOFF, 0);
#endif
    }
    else
    {
        m_busbhub2 = true;
        ui->usbHub2->setPalette(redPalette);
#ifdef __linux__
        ioctl(fd, IOCTL_USB1_ONOFF, 1);
#endif
    }
}

void CTestDialog::on_ioTest_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void CTestDialog::on_serialTest_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void CTestDialog::on_etcTest_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void CTestDialog::setupComm(int fd)
{
#ifdef __linux__
    termios newStatus;
    ::bzero(&newStatus, sizeof(newStatus)); /* clear struct for new port settings */
    newStatus.c_cflag &= ~CSIZE;
    newStatus.c_cflag |= CS8;

    newStatus.c_cflag &= ~(PARENB|PARODD);

    newStatus.c_cflag |= B9600 |  CLOCAL | CREAD ;
    newStatus.c_iflag = 0;
    newStatus.c_oflag = 0;
    newStatus.c_cc[VTIME]    = 1;   /* 문자 사이의 timer를 disable */
    newStatus.c_cc[VMIN]     = 1;
    ::tcflush(fd, TCIFLUSH); //버퍼비우기

    ::tcsetattr(fd, TCSANOW, &newStatus);
#endif
}

void CTestDialog::on_serialClear_clicked()
{
    ui->com1Tx->setText("abcdefghijk1234567890");
    ui->com2Tx->setText("abcdefghijk1234567890");
    ui->com3Tx->setText("abcdefghijk1234567890");
    ui->com4Tx->setText("abcdefghijk1234567890");
    ui->com5Tx->setText("abcdefghijk1234567890");
    ui->com1Rx->setText("");
    ui->com2Rx->setText("");
    ui->com3Rx->setText("");
    ui->com4Rx->setText("");
    ui->com5Rx->setText("");
}

void CTestDialog::openComm(int& fd, QString msg, int& timer)
{
#ifdef __linux__
    if(fd >= 0)
    {
        ::write(fd, msg.toLocal8Bit().data(), msg.length());
        if(timer != 0)
            killTimer(timer);
        timer = startTimer(1);
    }
#endif
}

void CTestDialog::readComm(int& fd, QString send, int& timer, QLineEdit *edit)
{
    killTimer(timer);
    timer = 0;
#ifdef __linux__
    QString rcv;
    char buf[50];
    int len = ::read(fd, buf, 50);
    if(len > 0)
    {
        buf[len] = 0;
        rcv = buf;
        edit->setText(rcv);
    }
#endif
}


void CTestDialog::on_com1Test_clicked()
{
    QString msg = ui->com1Tx->text();
    ui->com1Rx->setText("");
    openComm(m_Fd1, msg, m_timerIdCom1);
}
void CTestDialog::on_com2Test_clicked()
{
    QString msg = ui->com2Tx->text();
    ui->com2Rx->setText("");
    openComm(m_Fd2, msg, m_timerIdCom2);
}

void CTestDialog::on_com3Test_clicked()
{
    QString msg = ui->com3Tx->text();
    ui->com3Rx->setText("");
    openComm(m_Fd3, msg, m_timerIdCom3);
}

void CTestDialog::on_com4Com5Test_clicked()
{
    QString msg = ui->com4Tx->text();
    ui->com5Rx->setText("");
    openComm(m_Fd4, msg, m_timerIdCom4Com5);
}

void CTestDialog::on_com5Com4Test_clicked()
{
    QString msg = ui->com5Tx->text();
    ui->com4Rx->setText("");
    openComm(m_Fd5, msg, m_timerIdCom5Com4);
}

void dump(const void *s_data, int s_size, char *fmt,...)
{
    va_list ap;
    va_start(ap, fmt);

    char title[1024];
    char format[1024];
    if(fmt == NULL){
        (void)fprintf(stdout, "============>%s:%d<===========\n", title, s_size);
    }else{
        snprintf(format, 1024, "============>%s:%d<===========\n", fmt, s_size);
        vsnprintf(title, 1024, format, ap);
        (void)fprintf(stdout, title);
    }

    int s_o,s_w,s_i;
    unsigned char s_b[17];

    s_b[16] = '\0';
    s_o = (int)0;

    while(s_o<(s_size)){
        s_w=((s_size)-s_o)<16?((s_size)-s_o):16;
        printf("%08X",s_o);
        for(s_i=0;s_i<s_w;s_i++){
            if(s_i==8)
                printf(" | ");
            else
                printf(" ");
            s_b[s_i]=*(((unsigned char *)(s_data))+s_o+s_i);
            printf("%02X",s_b[s_i]);
            if((s_b[s_i]&0x80)||(s_b[s_i]<' '))
                s_b[s_i]='.';
        }
        while(s_i<16){
            if(s_i==8)
                printf("     ");
            else
                printf("   ");
            s_b[s_i]=' ';s_i++;
        }
        printf(" [%s]\n",(char *)s_b);
        s_o+=16;
    }
    (void)fprintf(stdout, "\n");
    va_end(ap);
}
int uart3=-1,uart4=-1, uart5=-1;
void CTestDialog::on_touch_clicked()
{

    if((uart3= ::open(ComStr[3], O_RDWR)) == -1)
      {
      //    printf("cannot open touch device %s\n", TOUCH_DEVICE);
      //    exit(EXIT_FAILURE);
      }

      if((uart4= ::open(ComStr[4], O_RDWR)) == -1)
      {
      //    printf("cannot open touch device %s\n", TOUCH_DEVICE);
      //    exit(EXIT_FAILURE);
      }


      if((uart5= ::open(ComStr[5], O_RDWR)) == -1)
      {
//          printf("cannot open touch device %s\n", TOUCH_DEVICE);
 //         exit(EXIT_FAILURE);
      }
    CTouchTest dlg;
    dlg.exec();
    ::close(uart3);
    ::close(uart4);
    ::close(uart5);
    uart3=uart4=uart5=-1;
}

void CTestDialog::on_lcd_clicked()
{
    CLcdTestDialog dlg;
    dlg.exec();
}

void CTestDialog::aiCalibrationDataWrite()
{
    memcpy(AdcCalibraion, m_AdcCalibraion, sizeof(AdcCalibraion));
    QJsonObject argObject;
    QJsonArray array;
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            QJsonValue val(AdcCalibraion[i][j]);
            array.append(val);
        }
    }
    QJsonValue vValue(array);
    CMonitorUdp::Instance()->sendCommand(this, QString("System_SetAdcCalibration"), argObject, vValue);
}

void CTestDialog::setText(QPushButton *button, int ai, int pos)
{
    QString txt;
    txt = QString("AI%1 %2").arg(ai).arg(m_AdcCalibraion[ai-1][pos]);

    button->setText(txt);
}

void CTestDialog::on_ai1_4_clicked()
{
    m_AdcCalibraion[0][0] = m_Value[0];
    setText(ui->ai1_4, 1, 0);
}

void CTestDialog::on_ai2_4_clicked()
{
    m_AdcCalibraion [1][0] = m_Value[1];
    setText(ui->ai2_4, 2, 0);
}

void CTestDialog::on_ai3_4_clicked()
{
    m_AdcCalibraion [2][0] = m_Value[2];
    setText(ui->ai3_4, 3, 0);
}

void CTestDialog::on_ai4_4_clicked()
{
    m_AdcCalibraion [3][0] = m_Value[3];
    setText(ui->ai4_4, 4, 0);
}

void CTestDialog::on_ai5_4_clicked()
{
    m_AdcCalibraion [4][0] = m_Value[4];
    setText(ui->ai5_4, 5, 0);
}

void CTestDialog::on_ai6_4_clicked()
{
    m_AdcCalibraion [5][0] = m_Value[5];
    setText(ui->ai6_4, 6, 0);
}

void CTestDialog::on_ai1_20_clicked()
{
    m_AdcCalibraion [0][1] = m_Value[0];
    setText(ui->ai1_20, 1, 1);
}

void CTestDialog::on_ai2_20_clicked()
{
    m_AdcCalibraion [1][1] = m_Value[1];
    setText(ui->ai2_20, 2, 1);
}

void CTestDialog::on_ai3_20_clicked()
{
    m_AdcCalibraion [2][1] = m_Value[2];
    setText(ui->ai3_20, 3, 1);
}

void CTestDialog::on_ai4_20_clicked()
{
    m_AdcCalibraion [3][1] = m_Value[3];
    setText(ui->ai4_20, 4, 1);
}

void CTestDialog::on_ai5_20_clicked()
{
    m_AdcCalibraion [4][1] = m_Value[4];
    setText(ui->ai5_20, 5, 1);
}

void CTestDialog::on_ai6_20_clicked()
{
    m_AdcCalibraion [5][1] = m_Value[5];
    setText(ui->ai6_20, 6, 1);
}

void CTestDialog::on_tnSave_clicked()
{
    CQuestionDialog dlg(tr("저장하시겠습니까?"));
    if(dlg.exec() ==QDialog::Accepted)
        aiCalibrationDataWrite();
}

void CTestDialog::on_fram_clicked()
{
    ui->listWidgetMsg->clear();
#ifdef __linux__
    QString msg;

    char buf[20];
    char backup[20];
    int fd = ::open("/sys/devices/platform/fdd40000.i2c/i2c-0/0-0050/eeprom",  O_RDWR);
    if (fd < 0) {
        msg = "ERROR: EEPROM SPI open failed";
        msgPut(msg);
        return;
    }

//    ::lseek(fd, 0, SEEK_SET);
//    ::read(fd, backup, 10);

    ::lseek(fd, 2030, SEEK_SET);
    ::write(fd, "abcde12345", 10);

    ::lseek(fd, 2030, SEEK_SET);
    ::read(fd, buf, 10);

    buf[10]  = 0;

    if(::strncmp(buf, "abcde12345", 10) == 0)
        msg = "EEPROM test successfull ";
    else
        msg = "EEPROM test failed";
    msgPut(msg);
//    ::lseek(fd, 0, SEEK_SET);
//    ::write(fd, backup, 10);
    ::close(fd);
#endif
}

void CTestDialog::on_usbSerial_clicked()
{
    ui->listWidgetMsg->clear();
    ui->listWidgetMsg->clear();
    msgPut(QString("USB Serial Writing....."));
    system("setusbserial > /tmp/setusb.txt");
    messageRead("/tmp/setusb.txt", 0);
}

void CTestDialog::on_rtc_clicked()
{
    ::close(mRtc);
    ui->listWidgetMsg->addItem(QString("RTC를 시간서버와 동기화합니다."));
    system("date > /tmp/rtc.txt");
    messageRead("/tmp/rtc.txt", 0);
    system("rdate -s time.bora.net");
    system("hwclock -wu");
    ui->listWidgetMsg->addItem(QString("동기화한 시간"));
    system("date > /tmp/rtc.txt");
    mRtc = ::open ( "/dev/rtc0", O_RDWR);

    messageRead("/tmp/rtc.txt", 0);
}

void CTestDialog::on_ping_clicked()
{
    ui->listWidgetMsg->clear();
    msgPut(QString("LAN1 Testing....."));
    QString ping = QString("ping -c 3 -W 1 %1 > /tmp/ping.txt").arg(ui->lan1->text());
    system(ping.toLocal8Bit().data());
    messageRead("/tmp/ping.txt", 2);
    /*
#ifdef __linux__
    msgPut(QString("Ping Test Start"));
    QProcess ping;
    ping.start("ping", QStringList() << "-c 100 -f " << "192.168.123.254");
    if(ping.waitForFinished(3000) ) {
        while(ping.canReadLine()) {
            QString line = ping.readLine();
            msgPut(line);
        }
    }
#endif*/
}

void CTestDialog::on_pingLan2_clicked()
{
    ui->listWidgetMsg->clear();
    msgPut(QString("LAN2 Testing....."));
    QString ping = QString("ping -c 3 -W 1 %1 > /tmp/ping2.txt").arg(ui->lan2->text());
    system(ping.toLocal8Bit().data());
    messageRead("/tmp/ping2.txt", 2);
}

void CTestDialog::messageRead(const char *fname, int lineSkip)
{
    QFile file(fname);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray ar;
    for(int i = 0; i < lineSkip; i++)
        ar = file.readLine();
    ar = file.readAll();
    ui->listWidgetMsg->addItem(QString(ar.data()));
}
/*
void CTestDialog::on_CommTest_clicked()
{
    if(ui->Com1->isChecked())
    {
        if(gHardwareModel == "SS-100")
        {
            CommTxSeleced = 0;
            CommRxSeleced = 1;
        }
        else
        {
            CommTxSeleced = 0;
            CommRxSeleced = 0;
        }
    }
    else
    if(ui->Com2->isChecked())
    {
        if(gHardwareModel == "SS-100")
        {
            CommTxSeleced = 1;
            CommRxSeleced = 0;
        }
        else
        {
            CommTxSeleced = 1;
            CommRxSeleced = 1;
        }
    }
    else
    if(ui->Com3->isChecked())
    {
        CommTxSeleced = 2;
        CommRxSeleced = 2;
    }
    else
    if(ui->Com4->isChecked())
    {
        CommTxSeleced = 3;
        CommRxSeleced = 3;
    }
    else
    if(ui->Com5->isChecked())
    {
        CommTxSeleced = 4;
        CommRxSeleced = 4;
    }
    else
    if(ui->Com6->isChecked())
    {
        CommTxSeleced = 5;
        CommRxSeleced = 5;
    }
    else
    if(ui->Com7->isChecked())
    {
        CommTxSeleced = 6;
        CommRxSeleced = 6;
    }
    else
    if(ui->Com8->isChecked())
    {
        CommTxSeleced = 7;
        CommRxSeleced = 7;
    }
    else
    if(ui->Com9Com10->isChecked())
    {
        if(gHardwareRevision == "1")
        {
            CommTxSeleced = 8;
            CommRxSeleced = 8;
        }
        else
        {
            CommTxSeleced = 8;
            CommRxSeleced = 9;
        }
    }
    else
    if(ui->Com10Com9->isChecked())
    {
        if(gHardwareRevision == "1")
        {
            CommTxSeleced = 9;
            CommRxSeleced = 9;
        }
        else
        {
            CommTxSeleced = 9;
            CommRxSeleced = 8;
        }
    }
    ui->commRx->setText("");
    QString msg = ui->commTx->text();
    openComm(m_Fd[CommTxSeleced], msg, m_timerIdCommon);
}
*/
void CTestDialog::on_CommClear_clicked()
{
    ui->commTx->setText("abcdefghijk1234567890");
    ui->commRx->setText("");
}

void CTestDialog::on_btnOK_clicked()
{
    if(gHardwareModel == "SS-500" || gHardwareModel == "SS-501")
    {
#ifdef __linux__
        ::close(m_Fd1);
        ::close(m_Fd2);
        ::close(m_Fd3);
        ::close(m_Fd4);
        ::close(m_Fd5);
        ioctl(fd, IOCTL_USB1_ONOFF, 1);
        ioctl(fd, IOCTL_USB2_ONOFF, 1);
#endif
    }
    if(gHardwareModel == "SS-100")
    {
#ifdef __linux__
        for(int i = 0; i < 3; i++)
            if(m_Fd[i])
                ::close(m_Fd[i]);
        ioctl(fd, IOCTL_USB1_ONOFF, 1);
        ioctl(fd, IOCTL_USB2_ONOFF, 1);
#endif
    }
    else
    {
#ifdef __linux__
        for(int i = 0; i < 10; i++)
            if(m_Fd[i])
                ::close(m_Fd[i]);
        ioctl(fd, IOCTL_USB1_ONOFF, 1);
        ioctl(fd, IOCTL_USB2_ONOFF, 1);
        ioctl(fd, IOCTL_USB3_ONOFF, 1);
#endif
    }

#ifdef __linux__
    killTimer(m_timerId);
#endif
    killTimer(m_timerIdAi);

    ::close(fd);
    ::close(afd);
    ::close(mRtc);
    accept();
}


void CTestDialog::OpenNSend()
{
    ui->commRx->setText("");
    QString msg = ui->commTx->text();
    openComm(m_Fd[CommTxSeleced], msg, m_timerIdCommon);
}

void CTestDialog::on_Com1_clicked()
{
    if(gHardwareModel == "SS-100")
    {
        CommTxSeleced = 0;
        CommRxSeleced = 1;
    }
    else
    {
        CommTxSeleced = 0;
        CommRxSeleced = 0;
    }
    OpenNSend();
}

void CTestDialog::on_Com2_clicked()
{
    if(gHardwareModel == "SS-100")
    {
        CommTxSeleced = 1;
        CommRxSeleced = 0;
    }
    else
    {
        CommTxSeleced = 1;
        CommRxSeleced = 1;
    }
    OpenNSend();
}

void CTestDialog::on_Com3_clicked()
{
    CommTxSeleced = 2;
    CommRxSeleced = 2;
    OpenNSend();
}

void CTestDialog::on_Com4_clicked()
{
    CommTxSeleced = 3;
    CommRxSeleced = 3;
    OpenNSend();
}

void CTestDialog::on_Com5_clicked()
{
    CommTxSeleced = 4;
    CommRxSeleced = 4;
    OpenNSend();
}

void CTestDialog::on_Com6_clicked()
{
    CommTxSeleced = 5;
    CommRxSeleced = 5;
    OpenNSend();
}

void CTestDialog::on_Com7_clicked()
{
    CommTxSeleced = 6;
    CommRxSeleced = 6;
    OpenNSend();
}

void CTestDialog::on_Com8_clicked()
{
    CommTxSeleced = 7;
    CommRxSeleced = 7;
    OpenNSend();
}

void CTestDialog::on_Com11Com12_clicked()
{
    CommTxSeleced = 10;
    CommRxSeleced = 11;
    OpenNSend();
}

void CTestDialog::on_Com12Com11_clicked()
{
    CommTxSeleced = 11;
    CommRxSeleced = 10;
    OpenNSend();
}

void CTestDialog::onTimer()
{
}


void CTestDialog::on_btnDefault_clicked()
{
    for(int i = 0; i < 6; i++)
    {
        m_AdcCalibraion[i][0] = 637;
    }
    setText(ui->ai1_4, 1, 0);
    setText(ui->ai2_4, 2, 0);
    setText(ui->ai3_4, 3, 0);
    setText(ui->ai4_4, 4, 0);
    setText(ui->ai5_4, 5, 0);
    setText(ui->ai6_4, 6, 0);

    for(int i = 0; i < 6; i++)
    {
        m_AdcCalibraion [i][1] = 3198;
    }
    setText(ui->ai1_20, 1, 1);
    setText(ui->ai2_20, 2, 1);
    setText(ui->ai3_20, 3, 1);
    setText(ui->ai4_20, 4, 1);
    setText(ui->ai5_20, 5, 1);
    setText(ui->ai6_20, 6, 1);
}

void CTestDialog::on_Com9_clicked()
{
    CommTxSeleced = 8;
    CommRxSeleced = 8;
    OpenNSend();
}

void CTestDialog::on_Com10_clicked()
{
    CommTxSeleced = 9;
    CommRxSeleced = 9;
    OpenNSend();
}
