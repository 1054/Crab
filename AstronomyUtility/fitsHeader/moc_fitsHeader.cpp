/****************************************************************************
** Meta object code from reading C++ file 'fitsHeader.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "fitsHeader.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fitsHeader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_fitsHeader_t {
    QByteArrayData data[8];
    char stringdata[88];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_fitsHeader_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_fitsHeader_t qt_meta_stringdata_fitsHeader = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 8),
QT_MOC_LITERAL(2, 20, 0),
QT_MOC_LITERAL(3, 21, 10),
QT_MOC_LITERAL(4, 32, 4),
QT_MOC_LITERAL(5, 37, 24),
QT_MOC_LITERAL(6, 62, 7),
QT_MOC_LITERAL(7, 70, 16)
    },
    "fitsHeader\0openFile\0\0searchText\0text\0"
    "QTextDocument::FindFlags\0options\0"
    "isReHighlighting\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_fitsHeader[] = {

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
       1,    0,   39,    2, 0x0a,
       3,    0,   40,    2, 0x0a,
       3,    3,   41,    2, 0x0a,
       3,    2,   48,    2, 0x2a,
       3,    1,   53,    2, 0x2a,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 5, QMetaType::Bool,    4,    6,    7,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 5,    4,    6,
    QMetaType::Void, QMetaType::QString,    4,

       0        // eod
};

void fitsHeader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        fitsHeader *_t = static_cast<fitsHeader *>(_o);
        switch (_id) {
        case 0: _t->openFile(); break;
        case 1: _t->searchText(); break;
        case 2: _t->searchText((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QTextDocument::FindFlags(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 3: _t->searchText((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QTextDocument::FindFlags(*)>(_a[2]))); break;
        case 4: _t->searchText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject fitsHeader::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_fitsHeader.data,
      qt_meta_data_fitsHeader,  qt_static_metacall, 0, 0}
};


const QMetaObject *fitsHeader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *fitsHeader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_fitsHeader.stringdata))
        return static_cast<void*>(const_cast< fitsHeader*>(this));
    return QWidget::qt_metacast(_clname);
}

int fitsHeader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
