/****************************************************************************
** Meta object code from reading C++ file 'mySearchDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mySearchDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mySearchDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_mySearchDialog_t {
    QByteArrayData data[10];
    char stringdata[122];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_mySearchDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_mySearchDialog_t qt_meta_stringdata_mySearchDialog = {
    {
QT_MOC_LITERAL(0, 0, 14),
QT_MOC_LITERAL(1, 15, 13),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 4),
QT_MOC_LITERAL(4, 35, 24),
QT_MOC_LITERAL(5, 60, 7),
QT_MOC_LITERAL(6, 68, 16),
QT_MOC_LITERAL(7, 85, 10),
QT_MOC_LITERAL(8, 96, 10),
QT_MOC_LITERAL(9, 107, 13)
    },
    "mySearchDialog\0searchingText\0\0text\0"
    "QTextDocument::FindFlags\0options\0"
    "isReHighlighting\0searchNext\0searchPrev\0"
    "searchChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_mySearchDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   44,    2, 0x06,
       1,    2,   51,    2, 0x26,
       1,    1,   56,    2, 0x26,

 // slots: name, argc, parameters, tag, flags
       7,    0,   59,    2, 0x0a,
       8,    0,   60,    2, 0x0a,
       9,    0,   61,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4, QMetaType::Bool,    3,    5,    6,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4,    3,    5,
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void mySearchDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        mySearchDialog *_t = static_cast<mySearchDialog *>(_o);
        switch (_id) {
        case 0: _t->searchingText((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QTextDocument::FindFlags(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->searchingText((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QTextDocument::FindFlags(*)>(_a[2]))); break;
        case 2: _t->searchingText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->searchNext(); break;
        case 4: _t->searchPrev(); break;
        case 5: _t->searchChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (mySearchDialog::*_t)(const QString & , QTextDocument::FindFlags , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&mySearchDialog::searchingText)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject mySearchDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_mySearchDialog.data,
      qt_meta_data_mySearchDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *mySearchDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *mySearchDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_mySearchDialog.stringdata))
        return static_cast<void*>(const_cast< mySearchDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int mySearchDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void mySearchDialog::searchingText(const QString & _t1, QTextDocument::FindFlags _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
