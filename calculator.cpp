/****************************************************************************
**
** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding a valid Qt License Agreement may use this file in
** accordance with the rights, responsibilities and obligations
** contained therein.  Please consult your licensing agreement or
** contact sales@trolltech.com if any conditions of this licensing
** agreement are not clear to you.
**
** Further information about Qt licensing is available at:
** http://www.trolltech.com/products/qt/licensing.html or by
** contacting info@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QLineEdit>
#include <QGridLayout>
#include <QEvent>
#include <QMouseEvent>
#include <math.h>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include "button.h"
#include "calculator.h"

extern bool    m_bScreenSave;

Calculator::Calculator(QString& val, KEY_TYPE type, QWidget *parent)
    : QDialog(parent)
{
    QRect rec(0,0,800,650);
    this->setGeometry(0,0,800,650);
    QRect rect2;
    rect2 = this->geometry();
    m_Type = type;
    sumInMemory = 0.0;
    sumSoFar = 0.0;
    factorSoFar = 0.0;
    waitingForOperand = true;

    display = new QLineEdit(val, this);
    if(m_Type == CALCULATOR)
    {
        display->setReadOnly(true);
        display->setAlignment(Qt::AlignRight);
    }
    QFont font = this->font();
    font.setPointSize(24);
    this->setFont(font);
    display->setFont(font);
    display->setMaxLength(20);
    display->installEventFilter(this);

    font = this->font();
    font.setPointSize(32);
    this->setFont(font);
    if(m_Type != CALCULATOR)
        display->selectAll();

    QColor digitColor(150, 205, 205);
    QColor backspaceColor(225, 185, 135);
    QColor memoryColor(100, 155, 155);
    QColor operatorColor(155, 175, 195);

    for (int i = 0; i < NumDigitButtons; ++i) {
	digitButtons[i] = createButton(QString::number(i), digitColor,
                                       SLOT(digitClicked()));
    }

    pointButton = createButton(tr("."), digitColor, SLOT(pointClicked()));

    backspaceButton = createButton(tr("Backspace"), backspaceColor,
                                   SLOT(backspaceClicked()));
    clearButton = createButton(tr("Clear"), backspaceColor, SLOT(clear()));
    if(m_Type != IPKEY)
    {
        changeSignButton = createButton(tr("±"), digitColor, SLOT(changeSignClicked()));
    }

    okButton = createButton(tr("OK"), backspaceColor,
                                   SLOT(accept()));
    cancelButton = createButton(tr("Cancel"), backspaceColor,
                                   SLOT(reject()));

    QGridLayout *mainLayout = new QGridLayout(this);
   // mainLayout->setSizeConstraint(QLayout::SetFixedSize);


    if(m_Type == NUMKEY)
    {
        mainLayout->addWidget(display, 0, 0, 1, 5);
        mainLayout->addWidget(backspaceButton, 1, 3, 1, 2);
        mainLayout->addWidget(clearButton, 2, 3, 1, 2); //row, col, rowspan, colspan
        for (int i = 1; i < NumDigitButtons; ++i) {
            int row = ((9 - i) / 3) + 1;
            int column = ((i - 1) % 3);
            mainLayout->addWidget(digitButtons[i], row, column);
        }
        mainLayout->addWidget(digitButtons[0], 4, 0, 1, 1);
        mainLayout->addWidget(pointButton, 4, 1, 1, 1);
        mainLayout->addWidget(changeSignButton, 4, 2, 1, 1);

        mainLayout->addWidget(cancelButton, 3, 3, 1, 2);
        mainLayout->addWidget(okButton, 4, 3, 1, 2);
    }
    else
    if(m_Type == CALCULATOR)
    {
        clearMemoryButton = createButton(tr("MC"), memoryColor,
                                         SLOT(clearMemory()));
        readMemoryButton = createButton(tr("MR"), memoryColor, SLOT(readMemory()));
        setMemoryButton = createButton(tr("MS"), memoryColor, SLOT(setMemory()));
        addToMemoryButton = createButton(tr("M+"), memoryColor,
                                         SLOT(addToMemory()));

     //   divisionButton = createButton(tr("\367"), operatorColor,
        divisionButton = createButton(tr("÷"), operatorColor,
                                      SLOT(multiplicativeOperatorClicked()));
    //    timesButton = createButton(tr("\327"), operatorColor,
        timesButton = createButton(tr("×"), operatorColor,
                                   SLOT(multiplicativeOperatorClicked()));
        minusButton = createButton(tr("-"), operatorColor,
                                   SLOT(additiveOperatorClicked()));
        plusButton = createButton(tr("+"), operatorColor,
                                  SLOT(additiveOperatorClicked()));

        squareRootButton = createButton(tr("Sqrt"), operatorColor,
                                        SLOT(unaryOperatorClicked()));
    //    powerButton = createButton(tr("x\262"), operatorColor,
        powerButton = createButton(tr("x²"), operatorColor,
                                   SLOT(unaryOperatorClicked()));
        reciprocalButton = createButton(tr("1/x"), operatorColor,
                                        SLOT(unaryOperatorClicked()));
        equalButton = createButton(tr("="), operatorColor.light(120),
                                   SLOT(equalClicked()));
        clearAllButton = createButton(tr("Clear All"), backspaceColor.light(120),
                                      SLOT(clearAll()));
        mainLayout->addWidget(display, 0, 0, 1, 6);
        mainLayout->addWidget(backspaceButton, 1, 0, 1, 2);
        mainLayout->addWidget(clearButton, 1, 2, 1, 2); //row, col, rowspan, colspan
        mainLayout->addWidget(clearAllButton, 1, 4, 1, 2);

        mainLayout->addWidget(clearMemoryButton, 2, 0);
        mainLayout->addWidget(readMemoryButton, 3, 0);
        mainLayout->addWidget(setMemoryButton, 4, 0);
        mainLayout->addWidget(addToMemoryButton, 5, 0);
        for (int i = 1; i < NumDigitButtons; ++i) {
            int row = ((9 - i) / 3) + 2;
            int column = ((i - 1) % 3) + 1;
            mainLayout->addWidget(digitButtons[i], row, column);
        }

        mainLayout->addWidget(digitButtons[0], 5, 1);
        mainLayout->addWidget(pointButton, 5, 2);
        mainLayout->addWidget(changeSignButton, 5, 3);

        mainLayout->addWidget(divisionButton, 2, 4);
        mainLayout->addWidget(timesButton, 3, 4);
        mainLayout->addWidget(minusButton, 4, 4);
        mainLayout->addWidget(plusButton, 5, 4);

        mainLayout->addWidget(squareRootButton, 2, 5);
        mainLayout->addWidget(powerButton, 3, 5);
        mainLayout->addWidget(reciprocalButton, 4, 5);
        mainLayout->addWidget(equalButton, 5, 5);
        mainLayout->addWidget(okButton, 6, 0, 1, 3);
        mainLayout->addWidget(cancelButton, 6, 3, 1, 3);
    }
    else
    if(m_Type == IPKEY)
    {
        btn192 = createButton(tr("192."), backspaceColor,
                                       SLOT(btn192Clicked()));
        btn168 = createButton(tr("168."), backspaceColor,
                                       SLOT(btn168Clicked()));
        btn123 = createButton(tr("123."), backspaceColor,
                                       SLOT(btn123Clicked()));
        btn255 = createButton(tr("255."), backspaceColor,
                                       SLOT(btn255Clicked()));

        mainLayout->addWidget(display, 0, 0, 1, 5);
        mainLayout->addWidget(backspaceButton, 1, 0, 1, 3);
        mainLayout->addWidget(clearButton, 1, 3, 1, 2); //row, col, rowspan, colspan
        for (int i = 1; i < NumDigitButtons; ++i) {
            int row = ((9 - i) / 3) + 2;
            int column = ((i - 1) % 3);
            mainLayout->addWidget(digitButtons[i], row, column);
        }
        mainLayout->addWidget(digitButtons[0], 5, 0, 1, 1);
        mainLayout->addWidget(pointButton, 5, 1, 1, 2);

        mainLayout->addWidget(btn192, 2, 3, 1, 2);
        mainLayout->addWidget(btn168, 3, 3, 1, 2);
        mainLayout->addWidget(btn123, 4, 3, 1, 2);
        mainLayout->addWidget(btn255, 5, 3, 1, 2);

        mainLayout->addWidget(okButton, 6, 0, 1, 3);
        mainLayout->addWidget(cancelButton, 6, 3, 1, 2);
    }
	setLayout(mainLayout);

    setWindowTitle(tr("Calculator"));
    installEventFilter(this);
    startTimer(1000);

    const QRect screen = QApplication::desktop()->screenGeometry();
//    QRect rec(0,0,491,437);
    this->move( screen.center() - rect().center() );
}


Calculator::~Calculator()
{
    for (int i = 0; i < NumDigitButtons; ++i) {
		delete digitButtons[i];
    }
	delete backspaceButton;
	delete clearButton;
    if(m_Type != IPKEY)
        delete changeSignButton;
    if(m_Type == CALCULATOR)
    {
        delete clearMemoryButton;
        delete readMemoryButton;
        delete setMemoryButton;
        delete addToMemoryButton;
        delete divisionButton;
        delete timesButton;
        delete minusButton;
        delete plusButton;
        delete squareRootButton;
        delete powerButton;
        delete reciprocalButton;
        delete equalButton;
        delete clearAllButton;
    }
	delete okButton;
    delete cancelButton;
    if(m_Type == IPKEY)
    {
        delete btn192;
        delete btn168;
        delete btn123;
        delete btn255;
    }
}

bool Calculator::eventFilter(QObject *target, QEvent *event)
{
    return false;
    if(event->type() == QEvent::MouseButtonPress)
    {
        QPixmap	pixmap;
        pixmap = QPixmap::grabWidget(this);
        QString name = "Calculator.png";
        pixmap.save(name);
        qWarning() << "Captured : " + name;
//        return false;
    }
    if (target == display) {
        if (event->type() == QEvent::MouseButtonPress
                || event->type() == QEvent::MouseButtonDblClick
                || event->type() == QEvent::MouseButtonRelease
                || event->type() == QEvent::ContextMenu) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->buttons() & Qt::LeftButton) {
                QPalette newPalette = palette();
                newPalette.setColor(QPalette::Base,
                                    display->palette().color(QPalette::Text));
                newPalette.setColor(QPalette::Text,
                                    display->palette().color(QPalette::Base));
                display->setPalette(newPalette);
            } else {
                display->setPalette(palette());
            }
            return true;
        }
    }
    return QDialog::eventFilter(target, event);
}

void Calculator::digitClicked()
{
    int w2 = width();
    int h2 = height();

    Button *clickedButton = qobject_cast<Button *>(sender());
    int digitValue = clickedButton->text().toInt();
    if(m_Type == CALCULATOR)
    {
//    if (display->text() == "0" && digitValue == 0.0)
//        return;

        if (waitingForOperand) {
            display->clear();
        waitingForOperand = false;
        }
        display->setText(display->text() + QString::number(digitValue));
    }
    else
        display->insert(QString::number(digitValue));
}

void Calculator::unaryOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();
    double result=0;

    if (clickedOperator == tr("Sqrt")) {
        if (operand < 0.0) {
            abortOperation();
            return;
        }
        result = sqrt(operand);
    } else if (clickedOperator == tr("x²")) {
        result = pow(operand, 2.0);
    } else if (clickedOperator == tr("1/x")) {
        if (operand == 0.0) {
	    abortOperation();
	    return;
        }
        result = 1.0 / operand;
    }
    display->setText(QString::number(result));
    waitingForOperand = true;
}

void Calculator::additiveOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
	    return;
        }
        display->setText(QString::number(factorSoFar));
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }

    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
	    return;
        }
        display->setText(QString::number(sumSoFar));
    } else {
        sumSoFar = operand;
    }

    pendingAdditiveOperator = clickedOperator;
    waitingForOperand = true;
}

void Calculator::multiplicativeOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
	    return;
        }
        display->setText(QString::number(factorSoFar));
    } else {
        factorSoFar = operand;
    }

    pendingMultiplicativeOperator = clickedOperator;
    waitingForOperand = true;
}

void Calculator::equalClicked()
{
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
	    return;
        }
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }
    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
	    return;
        }
        pendingAdditiveOperator.clear();
    } else {
        sumSoFar = operand;
    }

    display->setText(QString::number(sumSoFar));
    sumSoFar = 0.0;
    waitingForOperand = true;
}

void Calculator::pointClicked()
{
    if(m_Type == CALCULATOR)
    {
        if (waitingForOperand)
            display->setText("0");
        if (!display->text().contains("."))
            display->insert(tr("."));
        waitingForOperand = false;
    }
    else
    if(m_Type == NUMKEY)
    {
        if (!display->text().contains("."))
            display->insert(tr("."));
    }
    else
    if(m_Type == IPKEY)
        display->insert(tr("."));
}

void Calculator::btn192Clicked()
{
    display->insert(tr("192."));
}

void Calculator::btn168Clicked()
{
    display->insert(tr("168."));
}

void Calculator::btn123Clicked()
{
    display->insert(tr("123."));
}

void Calculator::btn255Clicked()
{
    display->insert(tr("255."));
}

void Calculator::changeSignClicked()
{
    QString text = display->text();
    double value = text.toDouble();

    if (value > 0.0) {
        text.prepend(tr("-"));
    } else if (value < 0.0) {
        text.remove(0, 1);
    }
    display->setText(text);
}

void Calculator::backspaceClicked()
{
    if(m_Type == CALCULATOR)
    {
        if (waitingForOperand)
            return;

        QString text = display->text();
        text.chop(1);
        if (text.isEmpty()) {
            if(m_Type != IPKEY)
                text = "0";
            waitingForOperand = true;
        }
        display->setText(text);
    }
    else
        display->backspace();
}

void Calculator::clear()
{
    if(m_Type == CALCULATOR)
    {
        if (waitingForOperand)
            return;

        if(m_Type != IPKEY)
            display->setText("0");
        else
            display->setText("");
        waitingForOperand = true;
    }
    else
        display->clear();
}

void Calculator::clearAll()
{
    sumSoFar = 0.0;
    factorSoFar = 0.0;
    pendingAdditiveOperator.clear();
    pendingMultiplicativeOperator.clear();
    display->setText("0");
    waitingForOperand = true;
}

void Calculator::clearMemory()
{
    sumInMemory = 0.0;
}

void Calculator::readMemory()
{
    display->setText(QString::number(sumInMemory));
    waitingForOperand = true;
}

void Calculator::setMemory()
{
    equalClicked();
    sumInMemory = display->text().toDouble();
}

void Calculator::addToMemory()
{
    equalClicked();
    sumInMemory += display->text().toDouble();
}

Button *Calculator::createButton(const QString &text, const QColor &color,
                                 const char *member)
{
    Button *button = new Button(text, color, this);
    connect(button, SIGNAL(clicked()), this, member);
    QFont font;
    font.setPointSize(20);
    button->setFont(font);
    return button;
}

void Calculator::abortOperation()
{
    clearAll();
    display->setText(tr("####"));
}

bool Calculator::calculate(double rightOperand, const QString &pendingOperator)
{
    if (pendingOperator == tr("+")) {
        sumSoFar += rightOperand;
    } else if (pendingOperator == tr("-")) {
        sumSoFar -= rightOperand;
    } else if (pendingOperator == tr("×")) {
        factorSoFar *= rightOperand;
    } else if (pendingOperator == tr("÷")) {
	if (rightOperand == 0.0)
	    return false;
	factorSoFar /= rightOperand;
    }
    return true;
}

void Calculator::accept()
{
    m_Value = display->text();
    QDialog::accept();
}

void Calculator::paintEvent(QPaintEvent *)
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

void Calculator::timerEvent(QTimerEvent *)
{
    if (m_bScreenSave == true)
        QDialog::reject();
}
