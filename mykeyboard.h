#ifndef MYKEYBOARD_H
#define MYKEYBOARD_H

#include <qdialog.h>
#include <qpixmap.h>
#include <QLineEdit>

extern uint Popup2ScreenSaveCount;

#ifndef QT_NO_SCREEN_KEYBOARD

class Button;
class MyKeyboard : public QDialog
{
	Q_OBJECT

public:
    MyKeyboard(QWidget *parent, const char *name, QString text, QLineEdit::EchoMode echomode, int maxlen);
    ~MyKeyboard();
    QString m_Text;

protected:
    bool    eventFilter(QObject *target, QEvent *event);
    void    paintEvent(QPaintEvent*);

private:
    Button *createButton(QString text, const QColor &color, const char *member, int fontsize=12);
signals:
	void	sigSelectKey(const QString&);

private slots:
    void keyClicked();
    void bsClicked();
    void capslockClicked();
    void spaceClicked();
    void enterClicked();
    void cancelClicked();

private:
    void timerEvent(QTimerEvent *);
    Button  *keyButtons[4][10];
    Button  *bsButton;
    Button  *capslockButton;
    Button  *spaceButton;
    Button  *enterButton;
    Button  *cancelButton;
    bool	m_bUpper;
    QString m_strKeyL[4];
    QString m_strKeyU[4];
    QLineEdit *m_edit;
};

#endif // QT_NO_SCREEN_KEYBOARD

#endif	// MYKEYBOARD_H
