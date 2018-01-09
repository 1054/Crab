/****************************************************************************
** Meta object code from reading C++ file 'CrabNetworkSpeedMonitor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CrabNetworkSpeedMonitor/CrabNetworkSpeedMonitor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CrabNetworkSpeedMonitor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CrabNetworkSpeedMonitor_t {
    QByteArrayData data[5];
    char stringdata0[119];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CrabNetworkSpeedMonitor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CrabNetworkSpeedMonitor_t qt_meta_stringdata_CrabNetworkSpeedMonitor = {
    {
QT_MOC_LITERAL(0, 0, 23), // "CrabNetworkSpeedMonitor"
QT_MOC_LITERAL(1, 24, 37), // "update_network_speed_display_..."
QT_MOC_LITERAL(2, 62, 0), // ""
QT_MOC_LITERAL(3, 63, 26), // "update_interval_in_seconds"
QT_MOC_LITERAL(4, 90, 28) // "update_network_speed_display"

    },
    "CrabNetworkSpeedMonitor\0"
    "update_network_speed_display_interval\0"
    "\0update_interval_in_seconds\0"
    "update_network_speed_display"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CrabNetworkSpeedMonitor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a /* Public */,
       1,    0,   32,    2, 0x2a /* Public | MethodCloned */,
       4,    0,   33,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CrabNetworkSpeedMonitor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CrabNetworkSpeedMonitor *_t = static_cast<CrabNetworkSpeedMonitor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->update_network_speed_display_interval((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->update_network_speed_display_interval(); break;
        case 2: _t->update_network_speed_display(); break;
        default: ;
        }
    }
}

const QMetaObject CrabNetworkSpeedMonitor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CrabNetworkSpeedMonitor.data,
      qt_meta_data_CrabNetworkSpeedMonitor,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CrabNetworkSpeedMonitor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CrabNetworkSpeedMonitor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CrabNetworkSpeedMonitor.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CrabNetworkSpeedMonitor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
