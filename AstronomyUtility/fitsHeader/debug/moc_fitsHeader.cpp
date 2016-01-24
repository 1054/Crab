/****************************************************************************
** Meta object code from reading C++ file 'fitsHeader.h'
**
** Created: Sun Sep 16 01:21:31 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../fitsHeader.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fitsHeader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_fitsHeader[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      23,   11,   11,   11, 0x0a,
      66,   36,   11,   11, 0x0a,
     129,  116,   11,   11, 0x2a,
     179,  174,   11,   11, 0x2a,

       0        // eod
};

static const char qt_meta_stringdata_fitsHeader[] = {
    "fitsHeader\0\0openFile()\0searchText()\0"
    "text,options,isReHighlighting\0"
    "searchText(QString,QTextDocument::FindFlags,bool)\0"
    "text,options\0searchText(QString,QTextDocument::FindFlags)\0"
    "text\0searchText(QString)\0"
};

void fitsHeader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
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

const QMetaObjectExtraData fitsHeader::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject fitsHeader::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_fitsHeader,
      qt_meta_data_fitsHeader, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &fitsHeader::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *fitsHeader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *fitsHeader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_fitsHeader))
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
    }
    return _id;
}
QT_END_MOC_NAMESPACE
