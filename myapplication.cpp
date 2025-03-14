//#include <QSound>
#include <QEvent>
#include <QDebug>
#include <QFile>
#include <QGraphicsSceneMouseEvent>
#include "myapplication.h"

#include<stdio.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include "srview.h"
#include "epsene.h"
#include "tmsdialog.h"

#ifdef __linux__
#include <sys/ioctl.h>
#include <termios.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/reboot.h>
#endif
int     sr_fd;
bool    m_bScreenSave;
extern CSrView	*g_pMainView;
extern QObject *g_View;
extern QString gHardwareModel;
extern QString gSoftwareModel;

extern int uart3,uart4, uart5;

unsigned char color_buffer[5][4] =
{
    {0x00, 0x00, 0x00, 0xff},   // balck dark=0x80
    {0x00, 0x00, 0xff, 0xff},   // red
    {0x00, 0xff, 0x00, 0xff},   // green
    {0xff, 0x00, 0x00, 0xff},   // blue
    {0xff, 0xff, 0xff, 0xff}    // white
};
int color_index=0;
int screen_save_count;

CMyApplication::CMyApplication(int argc, char *argv[]) : QApplication(argc, argv)
{
    m_nIdle = 0;
    m_bScreenSave = false;
    int flag = O_RDWR;
#ifdef __linux__
    flag |= O_NDELAY;
#endif
    sr_fd = ::open("/dev/iostardust", flag);
    m_timerId = startTimer(1000);
    m_timerIdReset = startTimer(100);
    m_time.start();
 //   m_nTouchResetCount = 0;
    BacklightControlOn(1);
}

CMyApplication::~CMyApplication()
{
    killTimer(m_timerId);
    ::close(sr_fd);
}

void CMyApplication::ScreenSaveDisplay()
{
    if(color_index >= 5 || color_index < 0)
        color_index = 0;
    int fd;
    if((fd = open ("/dev/fb0", O_WRONLY)) == -1)
            return;
    unsigned char *buf = (unsigned char *)malloc(1920*4*1200);

    int pos = 0;
    for(int row = 0; row < 1200; row++)
    {
        for (int col = 0; col < 1920; col++)
        {
            memcpy(&buf[pos], &color_buffer[color_index], 4);
            pos += 4;
        }
    }
    write(fd, buf, 1920*4*1200);
    free(buf);
    close(fd);
    color_index++;
}

void CMyApplication::Update()
{
    if(gSoftwareModel == "EPS_HANRIVER")    // 한강수계 표준RTU
    {
        ((EpsEnE*)g_View )->update();
    }
    else
    if(gSoftwareModel == "ORION_TMS")    // 수질 TMS
    {
        ((TmsDialog*)g_View )->update();
    }
    else
    {
        ((CSrView*)g_View )->update();
    }
}

int aNameAdd;
bool CMyApplication::notify(QObject *receiver, QEvent *event)
{
    QPoint pos;
    if(event->type() == QEvent::MouseButtonPress)
    {
//        m_pSound->play();
        beep();
        if(uart3 != -1)
            ::write(uart3,  "01234567890123456789012345678900123456789012345678901234567890", 60);
        m_nIdle = 0;
  //      m_nTouchResetCount = 0;
        if(m_bScreenSave)
        {
            m_bScreenSave = false;
            Update();
            BacklightControlOn(1);
/*
            TouchReset();
            QGraphicsSceneMouseEvent pressEvent2(QEvent::GraphicsSceneMouseRelease);
//            QEvent pressEvent2(QEvent::MouseButtonRelease);
            pressEvent2.setScenePos(QPointF(0, 0));
            pressEvent2.setButton(Qt::LeftButton);
            pressEvent2.setButtons(Qt::LeftButton);
            sendEvent(this, &pressEvent2);
            processEvents();
*/            return false;
        }
    }
    else
    if(event->type() == QEvent::MouseMove)
    {
        if(uart4 != -1)
            ::write(uart4,  "0", 1);
        m_nIdle = 0;
        if(m_bScreenSave)
        {
            m_bScreenSave = false;
            Update();
            BacklightControlOn(1);
            return false;
        }
    }
    else
    if(event->type() == QEvent::MouseButtonRelease)
    {
        if(uart5 != -1)
            ::write(uart5,  "01234567890123456789012345678900123456789012345678901234567890", 60);
        m_nIdle = 0;
        if(m_bScreenSave)
        {
            m_bScreenSave = false;
            Update();
            BacklightControlOn(1);
            return false;
        }
    }
    return QApplication::notify(receiver, event);
}

void CMyApplication::timerEvent(QTimerEvent *event)
{
/*    if (event->timerId() == m_timerIdReset)
    {
        if(m_bReset)
        {
            QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
//            QEvent pressEvent(QEvent::MouseButtonPress);
            pressEvent.setScenePos(QPointF(0, 0));
            pressEvent.setButton(Qt::LeftButton);
            pressEvent.setButtons(Qt::LeftButton);
            sendEvent(this, &pressEvent);
            processEvents();
            QGraphicsSceneMouseEvent pressEvent2(QEvent::GraphicsSceneMouseRelease);
//            QEvent pressEvent2(QEvent::MouseButtonRelease);
            pressEvent2.setScenePos(QPointF(0, 0));
            pressEvent2.setButton(Qt::LeftButton);
            pressEvent2.setButtons(Qt::LeftButton);
            sendEvent(this, &pressEvent2);
            processEvents();
            m_bReset = false;
        }
    }
 */   if (event->timerId() == m_timerId)
    {
        if(m_bScreenSave == false)
        {
            if(m_bScreenSave == false && ++m_nIdle > 4*60*60)	// 30분
            {
                m_bScreenSave = true;
                BacklightControlOn(0);
                screen_save_count = 0;
                color_index = 0;
                ScreenSaveDisplay();
            }
        }
        else
        {
            if(++screen_save_count > 3)
            {
                ScreenSaveDisplay();
                screen_save_count = 0;
            }
        }
/*        if(++m_nTouchResetCount >= 60*60)
        {
            m_nTouchResetCount = 0;
            TouchReset();
        }
*/    }
}

void CMyApplication::BacklightControlOn(int on)
{

#ifdef __linux__
       ioctl(sr_fd, IOCTL_LCD_ONOFF, on);
#endif

}

void CMyApplication::beep()
{
    if(m_time.elapsed() < 100)
        return;
#ifdef __linux__
    ioctl(sr_fd, IOCTL_BEEP, 10);
#endif
    m_time.restart();
}

/*void CMyApplication::TouchReset()
{
    if(gHardwareModel == "SS-100")
        return;
#ifdef
__linux__
    ioctl(sr_fd, IOCTL_RESET_TOUCH, 10);
#endif
    QThread::msleep(200);
    system("echo 45 > /sys/devices/soc0/soc.0/2100000.aips-bus/21a8000.i2c/i2c-2/2-0038/threshold");
    m_bReset = true;
    *
    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMouseRelease);
    pressEvent.setScenePos(QPointF(0, 0));
    pressEvent.setButton(Qt::LeftButton);
    pressEvent.setButtons(Qt::LeftButton);
    sendEvent(this, &pressEvent);
    processEvents();
    system("echo 45 > /sys/devices/soc0/soc.0/2100000.aips-bus/21a8000.i2c/i2c-2/2-0038/threshold");
    *
}
*/
