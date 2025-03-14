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

#include <QtGui> 

#include "button.h"

Button::Button(const QString &text, const QColor &color, QWidget *parent)
    : QToolButton(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setText(text);

    QPalette newPalette = palette();
    newPalette.setColor(QPalette::Button, color);
    setPalette(newPalette);
	//QPen logpen;
	//logpen.setColor(color);
//	setPen(logpen);
}

QSize Button::sizeHint() const
{
    QSize size = QToolButton::sizeHint();
    size.rheight() += 10;
    size.rwidth() = qMax(size.width(), size.height());
    return size;
}

Button2::Button2(const QString &text, const QColor &color, QWidget *parent)
    : QToolButton(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setText(text);

    QPalette newPalette = palette();
    newPalette.setColor(QPalette::ButtonText, color);
    setPalette(newPalette);
    //QPen logpen;
    //logpen.setColor(color);
//	setPen(logpen);
}

QSize Button2::sizeHint() const
{
    QSize size = QToolButton::sizeHint();
    size.rheight() += 10;
    size.rwidth() = qMax(size.width(), size.height());
    return size;
}
