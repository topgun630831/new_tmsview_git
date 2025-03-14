/********************************************************************************
** Form generated from reading UI file 'digitalset2.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIGITALSET2_H
#define UI_DIGITALSET2_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QToolButton>

QT_BEGIN_NAMESPACE

class Ui_CDigitalSet2
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *editTagName;
    QLineEdit *editDesc;
    QGroupBox *groupBox;
    QRadioButton *btnOn;
    QRadioButton *btnOff;
    QToolButton *btnOk;
    QToolButton *btnCancel;

    void setupUi(QDialog *CDigitalSet2)
    {
        if (CDigitalSet2->objectName().isEmpty())
            CDigitalSet2->setObjectName(QStringLiteral("CDigitalSet2"));
        CDigitalSet2->resize(454, 362);
        QFont font;
        font.setPointSize(12);
        CDigitalSet2->setFont(font);
        label = new QLabel(CDigitalSet2);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 111, 21));
        QFont font1;
        font1.setPointSize(14);
        label->setFont(font1);
        label_2 = new QLabel(CDigitalSet2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 60, 101, 21));
        label_3 = new QLabel(CDigitalSet2);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 90, 101, 21));
        editTagName = new QLineEdit(CDigitalSet2);
        editTagName->setObjectName(QStringLiteral("editTagName"));
        editTagName->setGeometry(QRect(142, 60, 171, 30));
        editDesc = new QLineEdit(CDigitalSet2);
        editDesc->setObjectName(QStringLiteral("editDesc"));
        editDesc->setGeometry(QRect(140, 90, 301, 30));
        groupBox = new QGroupBox(CDigitalSet2);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(40, 150, 311, 80));
        btnOn = new QRadioButton(groupBox);
        btnOn->setObjectName(QStringLiteral("btnOn"));
        btnOn->setGeometry(QRect(30, 20, 93, 51));
        btnOff = new QRadioButton(groupBox);
        btnOff->setObjectName(QStringLiteral("btnOff"));
        btnOff->setGeometry(QRect(150, 20, 93, 51));
        btnOk = new QToolButton(CDigitalSet2);
        btnOk->setObjectName(QStringLiteral("btnOk"));
        btnOk->setGeometry(QRect(60, 260, 160, 60));
        btnCancel = new QToolButton(CDigitalSet2);
        btnCancel->setObjectName(QStringLiteral("btnCancel"));
        btnCancel->setGeometry(QRect(230, 260, 160, 60));

        retranslateUi(CDigitalSet2);
        QObject::connect(btnCancel, SIGNAL(clicked()), CDigitalSet2, SLOT(reject()));

        QMetaObject::connectSlotsByName(CDigitalSet2);
    } // setupUi

    void retranslateUi(QDialog *CDigitalSet2)
    {
        CDigitalSet2->setWindowTitle(QApplication::translate("CDigitalSet2", "Dialog", 0));
        label->setText(QApplication::translate("CDigitalSet2", "Digital Set", 0));
        label_2->setText(QApplication::translate("CDigitalSet2", "Tag \354\235\264\353\246\204", 0));
        label_3->setText(QApplication::translate("CDigitalSet2", "\354\204\244\353\252\205", 0));
        groupBox->setTitle(QApplication::translate("CDigitalSet2", "\354\240\234\354\226\264", 0));
        btnOn->setText(QApplication::translate("CDigitalSet2", "On", 0));
        btnOff->setText(QApplication::translate("CDigitalSet2", "Off", 0));
        btnOk->setText(QApplication::translate("CDigitalSet2", "\355\231\225\354\235\270", 0));
        btnCancel->setText(QApplication::translate("CDigitalSet2", "\354\267\250\354\206\214", 0));
    } // retranslateUi

};

namespace Ui {
    class CDigitalSet2: public Ui_CDigitalSet2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIGITALSET2_H
