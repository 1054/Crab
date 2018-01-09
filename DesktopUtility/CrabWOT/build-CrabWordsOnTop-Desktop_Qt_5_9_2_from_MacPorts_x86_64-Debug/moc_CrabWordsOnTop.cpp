/****************************************************************************
** Meta object code from reading C++ file 'CrabWordsOnTop.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CrabWordsOnTop/CrabWordsOnTop.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CrabWordsOnTop.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CrabWordsOnTop_t {
    QByteArrayData data[7];
    char stringdata0[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CrabWordsOnTop_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CrabWordsOnTop_t qt_meta_stringdata_CrabWordsOnTop = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CrabWordsOnTop"
QT_MOC_LITERAL(1, 15, 9), // "crab_edit"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 10), // "input_text"
QT_MOC_LITERAL(4, 37, 9), // "crab_move"
QT_MOC_LITERAL(5, 47, 14), // "crab_move_to_x"
QT_MOC_LITERAL(6, 62, 14) // "crab_move_to_y"

    },
    "CrabWordsOnTop\0crab_edit\0\0input_text\0"
    "crab_move\0crab_move_to_x\0crab_move_to_y"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CrabWordsOnTop[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x0a /* Public */,
       1,    0,   42,    2, 0x2a /* Public | MethodCloned */,
       4,    2,   43,    2, 0x0a /* Public */,
       4,    1,   48,    2, 0x2a /* Public | MethodCloned */,
       4,    0,   51,    2, 0x2a /* Public | MethodCloned */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal,    5,    6,
    QMetaType::Void, QMetaType::QReal,    5,
    QMetaType::Void,

       0        // eod
};

void CrabWordsOnTop::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CrabWordsOnTop *_t = static_cast<CrabWordsOnTop *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->crab_edit((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->crab_edit(); break;
        case 2: _t->crab_move((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 3: _t->crab_move((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 4: _t->crab_move(); break;
        default: ;
        }
    }
}

const QMetaObject CrabWordsOnTop::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CrabWordsOnTop.data,
      qt_meta_data_CrabWordsOnTop,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CrabWordsOnTop::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CrabWordsOnTop::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CrabWordsOnTop.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CrabWordsOnTop::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
