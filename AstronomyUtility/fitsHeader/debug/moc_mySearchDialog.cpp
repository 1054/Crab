/****************************************************************************
** Meta object code from reading C++ file 'mySearchDialog.h'
**
** Created: Sun Sep 16 01:21:35 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mySearchDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mySearchDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_mySearchDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      46,   16,   15,   15, 0x05,
     112,   99,   15,   15, 0x25,
     165,  160,   15,   15, 0x25,

 // slots: signature, parameters, type, tag, flags
     188,   15,   15,   15, 0x0a,
     201,   15,   15,   15, 0x0a,
     214,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_mySearchDialog[] = {
    "mySearchDialog\0\0text,options,isReHighlighting\0"
    "searchingText(QString,QTextDocument::FindFlags,bool)\0"
    "text,options\0"
    "searchingText(QString,QTextDocument::FindFlags)\0"
    "text\0searchingText(QString)\0searchNext()\0"
    "searchPrev()\0searchChanged()\0"
};

void mySearchDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
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
    }
}

const QMetaObjectExtraData mySearchDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject mySearchDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_mySearchDialog,
      qt_meta_data_mySearchDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &mySearchDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *mySearchDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *mySearchDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_mySearchDialog))
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
