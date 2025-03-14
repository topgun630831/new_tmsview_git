/****************************************************************************
** Meta object code from reading C++ file 'wmiview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "wmiview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wmiview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CWMIView_t {
    QByteArrayData data[21];
    char stringdata[237];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CWMIView_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CWMIView_t qt_meta_stringdata_CWMIView = {
    {
QT_MOC_LITERAL(0, 0, 8),
QT_MOC_LITERAL(1, 9, 10),
QT_MOC_LITERAL(2, 20, 0),
QT_MOC_LITERAL(3, 21, 7),
QT_MOC_LITERAL(4, 29, 14),
QT_MOC_LITERAL(5, 44, 12),
QT_MOC_LITERAL(6, 57, 15),
QT_MOC_LITERAL(7, 73, 14),
QT_MOC_LITERAL(8, 88, 4),
QT_MOC_LITERAL(9, 93, 5),
QT_MOC_LITERAL(10, 99, 15),
QT_MOC_LITERAL(11, 115, 11),
QT_MOC_LITERAL(12, 127, 11),
QT_MOC_LITERAL(13, 139, 9),
QT_MOC_LITERAL(14, 149, 10),
QT_MOC_LITERAL(15, 160, 14),
QT_MOC_LITERAL(16, 175, 11),
QT_MOC_LITERAL(17, 187, 11),
QT_MOC_LITERAL(18, 199, 12),
QT_MOC_LITERAL(19, 212, 16),
QT_MOC_LITERAL(20, 229, 7)
    },
    "CWMIView\0sigSysMenu\0\0onTimer\0"
    "onSysMenuTimer\0onTimerPopup\0slotDataChanged\0"
    "slotOpenWindow\0name\0nOpen\0slotCloseWindow\0"
    "slotTMSData\0slotTMSInfo\0slotSewer\0"
    "slotU_City\0slotRainSensor\0slotCommMon\0"
    "slotSysMenu\0slotPointMon\0slotAlarmSummary\0"
    "slotLog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CWMIView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,  100,    2, 0x08 /* Private */,
       4,    0,  101,    2, 0x08 /* Private */,
       5,    0,  102,    2, 0x08 /* Private */,
       6,    0,  103,    2, 0x08 /* Private */,
       7,    2,  104,    2, 0x0a /* Public */,
      10,    0,  109,    2, 0x0a /* Public */,
      11,    0,  110,    2, 0x0a /* Public */,
      12,    0,  111,    2, 0x0a /* Public */,
      13,    0,  112,    2, 0x0a /* Public */,
      14,    0,  113,    2, 0x0a /* Public */,
      15,    0,  114,    2, 0x0a /* Public */,
      16,    0,  115,    2, 0x0a /* Public */,
      17,    0,  116,    2, 0x0a /* Public */,
      18,    0,  117,    2, 0x0a /* Public */,
      19,    0,  118,    2, 0x0a /* Public */,
      20,    0,  119,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CWMIView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CWMIView *_t = static_cast<CWMIView *>(_o);
        switch (_id) {
        case 0: _t->sigSysMenu(); break;
        case 1: _t->onTimer(); break;
        case 2: _t->onSysMenuTimer(); break;
        case 3: _t->onTimerPopup(); break;
        case 4: _t->slotDataChanged(); break;
        case 5: _t->slotOpenWindow((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->slotCloseWindow(); break;
        case 7: _t->slotTMSData(); break;
        case 8: _t->slotTMSInfo(); break;
        case 9: _t->slotSewer(); break;
        case 10: _t->slotU_City(); break;
        case 11: _t->slotRainSensor(); break;
        case 12: _t->slotCommMon(); break;
        case 13: _t->slotSysMenu(); break;
        case 14: _t->slotPointMon(); break;
        case 15: _t->slotAlarmSummary(); break;
        case 16: _t->slotLog(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CWMIView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CWMIView::sigSysMenu)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject CWMIView::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CWMIView.data,
      qt_meta_data_CWMIView,  qt_static_metacall, 0, 0}
};


const QMetaObject *CWMIView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CWMIView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CWMIView.stringdata))
        return static_cast<void*>(const_cast< CWMIView*>(this));
    return QDialog::qt_metacast(_clname);
}

int CWMIView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void CWMIView::sigSysMenu()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
