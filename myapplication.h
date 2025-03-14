#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H
#include "QApplication"
#include <QTime>
#ifdef linux
#define IOCTL_SET           _IO('d', 0 )
#define IOCTL_BEEP          _IO('d', 1 )
#define IOCTL_LCD_ONOFF     _IO('d', 3) //
#define IOCTL_NAND_OFF      _IO('d', 5) //
#define IOCTL_USB1_ONOFF    _IO('d', 7) //
#define IOCTL_USB2_ONOFF    _IO('d', 9) //
#define IOCTL_USB3_ONOFF    _IO('d', 31) //
#define IOCTL_DI_PERIOD     _IO('d', 11) //
#define IOCTL_AI_PERIOD     _IO('d', 13) //
#define IOCTL_RESET         _IO('d', 15) //
#define IOCTL_DOUT_READ     _IO('d', 17) //
#define IOCTL_RESET_TOUCH   _IO('d', 45)
#endif

class QSound;
class CMyApplication : public QApplication
{
public:
    CMyApplication(int argc, char *argv[]);
    ~CMyApplication();
    void BacklightControlOn(int on);
 //   void TouchReset();

private:
    bool notify(QObject *receiver, QEvent *event);
    void timerEvent(QTimerEvent *);
    void beep();
    void ScreenSaveDisplay();
    void Update();
    QSound      *m_pSound;
    int         m_timerId;
    int         m_timerIdReset;
    int         m_nIdle;
    QTime       m_time;
//    int         m_nTouchResetCount;
    bool        m_bReset;
};

#endif // MYAPPLICATION_H
