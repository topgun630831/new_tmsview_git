#ifndef TESTDIALOG_H
#define TESTDIALOG_H

#include <QStackedWidget>
#include <QDialog>
#include <QLCDNumber>
#include <QLineEdit>
#include <QTimer>

namespace Ui {
class CTestDialog;
}

class CTestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CTestDialog(QWidget *parent = 0);
    ~CTestDialog();

    void msgPut(const QString& str);

private slots:
    void on_pushButtonClear_clicked();
    void doSetValue(int pos);
    void on_do1_clicked();
    void on_do2_clicked();
    void on_do3_clicked();
    void on_do4_clicked();
    void on_do5_clicked();
    void on_do6_clicked();
    void on_do7_clicked();
    void on_do8_clicked();
    void on_lcdOff_clicked();
    void on_usbHub1_clicked();
    void on_usbHub2_clicked();
    void on_ioTest_clicked();
    void on_serialTest_clicked();
    void on_etcTest_clicked();
    void setupComm(int fd);
    void on_serialClear_clicked();
    void openComm(int& fd, QString msg, int& timer);
    void readComm(int& fd, QString send,  int& tmer, QLineEdit *edit);
    void on_com1Test_clicked();
    void on_com2Test_clicked();
    void on_com3Test_clicked();
    void on_com4Com5Test_clicked();
    void on_com5Com4Test_clicked();
#ifdef linux
    int OpenI2c(int bus, int address);
    int WriteI2c(int fd, void *data, int dataOffset, int address, int len);
    int ReadI2c(int fd, void *data, int address, int len);

#endif

    void aiCalibrationDataWrite();
    void on_touch_clicked();
    void on_lcd_clicked();
    void on_ai1_4_clicked();
    void on_ai2_4_clicked();
    void on_ai3_4_clicked();
    void on_ai4_4_clicked();
    void on_ai5_4_clicked();
    void on_ai6_4_clicked();
    void on_ai1_20_clicked();
    void on_ai2_20_clicked();
    void on_ai3_20_clicked();
    void on_ai4_20_clicked();
    void on_ai5_20_clicked();
    void on_ai6_20_clicked();
    void on_tnSave_clicked();
    void on_fram_clicked();
    void on_usbSerial_clicked();
    void on_rtc_clicked();
    void on_ping_clicked();
    void on_pingLan2_clicked();
    void on_usbHub3_clicked();
//    void on_CommTest_clicked();
    void on_CommClear_clicked();
    void on_btnOK_clicked();

    void on_Com1_clicked();
    void on_Com2_clicked();
    void on_Com3_clicked();
    void on_Com4_clicked();
    void on_Com5_clicked();
    void on_Com6_clicked();
    void on_Com7_clicked();
    void on_Com8_clicked();
    void on_Com11Com12_clicked();
    void on_Com12Com11_clicked();
    void OpenNSend();
    void onTimer();

    void on_btnDefault_clicked();


    void on_Com9_clicked();

    void on_Com10_clicked();

protected:
    void timerEvent(QTimerEvent *);
    void doDisp(int value);
    void diDisp(int value);
    void aiDisp();

private:
    Ui::CTestDialog *ui;
    QPalette redPalette;
    QPalette greenPalette;
    int         m_timerId;
    int         m_timerIdAi;
    QPushButton *m_diButton[16];
    QPushButton *m_doButton[8];
    QLCDNumber   *m_aiLcd[6];
    QLCDNumber   *m_ai4Lcd[6];
    bool        m_busbhub1;
    bool        m_busbhub2;
    bool        m_busbhub3;
    int         m_Fd1;
    int         m_Fd2;
    int         m_Fd3;
    int         m_Fd4;
    int         m_Fd5;
    int         m_Fd[10];
    int         m_timerIdCom1;
    int         m_timerIdCom2;
    int         m_timerIdCom3;
    int         m_timerIdCom4Com5;
    int         m_timerIdCom5Com4;
    int         m_timerIdUsbSerial;
    int         m_timerIdCommon;
    int         m_timerIdPing;
    int         m_timerIdPing2;
    int         m_timerIdMpu;
    double      m_Value[6];
    double      m_AdcCalibraion[6][2];
    int         mRtc;
    int         CommRxSeleced;
    int         CommTxSeleced;
    int         mDiMax;
    void        setText(QPushButton *button, int ai, int pos);
    void        messageRead(const char *fname, int lineSkip);
    void	paintEvent(QPaintEvent*);
    QTimer              m_Timer;
};

#endif // TESTDIALOG_H
