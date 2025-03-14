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

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QDialog>

extern uint Popup2ScreenSaveCount;
typedef enum {
    NUMKEY=0,
    CALCULATOR=1,
    IPKEY=2
} KEY_TYPE;

class QLineEdit;
class Button;

class Calculator : public QDialog
{
    Q_OBJECT

public:
    Calculator(QString& val, KEY_TYPE = NUMKEY, QWidget *parent = 0);
    ~Calculator();
    QString	m_Value;
    KEY_TYPE m_Type;
protected:
    bool eventFilter(QObject *target, QEvent *event);
    void	paintEvent(QPaintEvent*);

private slots:
    void digitClicked();
    void unaryOperatorClicked();
    void additiveOperatorClicked();
    void multiplicativeOperatorClicked();
    void equalClicked();
    void pointClicked();
    void changeSignClicked();
    void backspaceClicked();
    void btn192Clicked();
    void btn168Clicked();
    void btn123Clicked();
    void btn255Clicked();
    void clear();
    void clearAll();
    void clearMemory();
    void readMemory();
    void setMemory();
    void addToMemory();

private:
    Button *createButton(const QString &text, const QColor &color,
                         const char *member);
    void abortOperation();
    bool calculate(double rightOperand, const QString &pendingOperator);
	void accept();
    void timerEvent(QTimerEvent *);

    double sumInMemory;
    double sumSoFar;
    double factorSoFar;
    QString pendingAdditiveOperator;
    QString pendingMultiplicativeOperator;
    bool waitingForOperand;

    QLineEdit *display;

    enum { NumDigitButtons = 10 };
    Button *digitButtons[NumDigitButtons];

    Button *pointButton;
    Button *changeSignButton;
    Button *backspaceButton;
    Button *clearButton;
    Button *clearAllButton;
    Button *clearMemoryButton;
    Button *readMemoryButton;
    Button *setMemoryButton;
    Button *addToMemoryButton;
   
    Button *divisionButton;
    Button *timesButton;
    Button *minusButton;
    Button *plusButton;
    Button *squareRootButton;
    Button *powerButton;
    Button *reciprocalButton;
    Button *equalButton;

    Button *okButton;
    Button *cancelButton;

    Button *btn192;
    Button *btn168;
    Button *btn123;
    Button *btn255;
};

#endif
