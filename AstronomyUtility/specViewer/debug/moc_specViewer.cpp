/****************************************************************************
** Meta object code from reading C++ file 'specViewer.h'
**
** Created: Fri Apr 27 20:35:48 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../specViewer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'specViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_specViewer[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      35,   23,   11,   11, 0x0a,
      57,   23,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_specViewer[] = {
    "specViewer\0\0openFile()\0strFilePath\0"
    "openFITSFile(QString)\0openASCIIFile(QString)\0"
};

const QMetaObject specViewer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_specViewer,
      qt_meta_data_specViewer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &specViewer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *specViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *specViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_specViewer))
        return static_cast<void*>(const_cast< specViewer*>(this));
    return QWidget::qt_metacast(_clname);
}

int specViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: openFile(); break;
        case 1: openFITSFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: openASCIIFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
