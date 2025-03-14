#include <QtGui>
#include <QPainter>
#include <QGridLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QDesktopWidget>
#include <QApplication>
#include "mykeyboard.h"
#include "button.h"

extern bool    m_bScreenSave;

#define	CX_BUTTON	40
#define	CY_BUTTON	40

#define KBD_WIDTH 680
#define KBD_HEIGHT 400


MyKeyboard::MyKeyboard(QWidget *parent, const char *name, QString text, QLineEdit::EchoMode echomode, int maxlen)
//        :QDialog(parent, name, modal, f)
        :QDialog(parent)
{
//	setCaption("Screen Keyboard");

    setWindowTitle(name);
    m_Text = text;
    // 400x200 으로 고정크기를 설정한다
    setGeometry(0,0,KBD_WIDTH,KBD_HEIGHT);
    setFixedSize(KBD_WIDTH, KBD_HEIGHT);

    QColor keyColor(150, 205, 205);
    QColor bsColor(225, 185, 135);
    QColor spaceColor(100, 155, 155);
    QColor enterColor(155, 175, 195);
    QColor cancelColor(255, 128, 192);

    m_strKeyL[0] = "1234567890";
    m_strKeyL[1] = "qwertyuiop";
    m_strKeyL[2] = "asdfghjkl:";
    m_strKeyL[3] = "zxcvbnm,.-";

    m_strKeyU[0] = "!@#$%^&*()";
    m_strKeyU[1] = "QWERTYUIOP";
    m_strKeyU[2] = "ASDFGHJKL;";
    m_strKeyU[3] = "ZXCVBNM<>?";

    int width = (KBD_WIDTH-10*11) / 10;
    int height = (KBD_HEIGHT-10*7) / 6;
    int left = (KBD_WIDTH-10*9 - width * 10) / 2;
    int y = (KBD_HEIGHT-10*5-height*6) / 2;
    int x = left;
    m_edit = new QLineEdit(this);
    m_edit->setGeometry(x, y+10, width*10+10*9, height-20);
    QFont font;
    font.setPointSize(14);
    m_edit->setFont(font);
    m_edit->setText(text);
    m_edit->selectAll();
    m_edit->setEchoMode(echomode);
    m_edit->setMaxLength(maxlen);
    for (int i = 0; i < 4; i++)
    {
        y += (height+10);
        x = left;
        for(int j = 0; j < 10; j++, x += (width+10))
        {
            keyButtons[i][j] = createButton(QString(m_strKeyL[i][j]), keyColor, SLOT(keyClicked()), 18);
            keyButtons[i][j]->setGeometry(x, y, width, height);
        }
    }
    x = left;
    y += (height+10);
    bsButton = createButton("BS", bsColor, SLOT(bsClicked()));
    bsButton->setGeometry(x, y, width*2+10, height);
    x += width*2+10*2;
    capslockButton = createButton("Capslock", enterColor.light(120), SLOT(capslockClicked()));
    capslockButton->setGeometry(x, y, width*2+10, height);
    capslockButton->setCheckable(true);
    x += width*2+10*2;
    spaceButton = createButton("Space", spaceColor, SLOT(spaceClicked()));
    spaceButton->setGeometry(x, y, width*3+10*2, height);
    x += width*3+10*3;
    enterButton = createButton("Enter", enterColor, SLOT(enterClicked()));
    enterButton->setGeometry(x, y, width*2+10*1, height);
    x += width*2+10*2;
    cancelButton = createButton("Cancel", bsColor.light(120), SLOT(cancelClicked()));
    cancelButton->setGeometry(x, y, width, height);
    // 키보드에서 선택한 문자를 부모 edit 위젯에 보낸다
    connect(this, SIGNAL(sigSelectKey(const QString&)), parent, SLOT(slotMyInsert(const QString&)));

	m_bUpper = false;

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );

    installEventFilter(this);
    startTimer(1000);
}

MyKeyboard::~MyKeyboard()
{
    for (int i = 0; i < 4; ++i)
        for(int j = 0; j < 10; j++)
            delete keyButtons[i][j];
    delete bsButton;
    delete capslockButton;
    delete spaceButton;
    delete enterButton;
    delete cancelButton;
    delete m_edit;
}

Button *MyKeyboard::createButton(QString text, const QColor &color, const char *member, int fontsize)
{
    text = " " + text + " ";
    Button *button = new Button(text, color, this);
    QFont font;
    font.setPointSize(fontsize);
    button->setFont(font);
    connect(button, SIGNAL( clicked()), this, member);
    return button;
}


bool MyKeyboard::eventFilter(QObject *target, QEvent *event)
{
    return false;
    if(event->type() == QEvent::MouseButtonPress)
    {
        QPixmap	pixmap;
        pixmap = QPixmap::grabWidget(this);
        QString name = "MyKeyboard.png";
        pixmap.save(name, "png"/* .toAscii()*/);
        qWarning() << "Captured : " + name;
    }
    return QDialog::eventFilter(target, event);
}

void MyKeyboard::keyClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    // 선택한 문자를 보낸다
    QString text = clickedButton->text();
    text = text.mid(1,1);
    m_edit->insert(text);
//    emit sigSelectKey(text);
}

void MyKeyboard::bsClicked()
{
    m_edit->backspace();
//    emit sigSelectKey(QString(QChar(0x08)));
}

void MyKeyboard::capslockClicked()
{
    if(m_bUpper)
        m_bUpper = false;
    else
        m_bUpper = true;
    for (int i = 0; i < 4; ++i)
        for(int j = 0; j < 10; j++)
        {
            QString str;
            if(m_bUpper)
                str = " " + QString(m_strKeyU[i][j]) + " ";
            else
                str = " " + QString(m_strKeyL[i][j]) + " ";
            keyButtons[i][j]->setText(str);
        }
    if(m_bUpper)
        keyButtons[0][6]->setText(QString("&&"));
}

void MyKeyboard::spaceClicked()
{
    m_edit->insert(QString(" "));
//    emit sigSelectKey(QString(" "));
}

void MyKeyboard::enterClicked()
{
    m_Text = m_edit->text();
    accept();
//    emit sigSelectKey(QString(QChar(0x0d)));
}

void MyKeyboard::cancelClicked()
{
    reject();
//    emit sigSelectKey(QString(QChar(0x0d)));
}

void MyKeyboard::paintEvent(QPaintEvent *)
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

void MyKeyboard::timerEvent(QTimerEvent *)
{
    if (m_bScreenSave == true)
        QDialog::reject();
}
