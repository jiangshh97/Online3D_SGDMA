/****************************************************************************
** Meta object code from reading C++ file 'abstractreconalgorithm.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/reconstructionProcess/abstractreconalgorithm.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'abstractreconalgorithm.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AbstractReconAlgorithm_t {
    QByteArrayData data[10];
    char stringdata0[121];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AbstractReconAlgorithm_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AbstractReconAlgorithm_t qt_meta_stringdata_AbstractReconAlgorithm = {
    {
QT_MOC_LITERAL(0, 0, 22), // "AbstractReconAlgorithm"
QT_MOC_LITERAL(1, 23, 21), // "sendCloudFitterResult"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 7), // "succeed"
QT_MOC_LITERAL(4, 54, 18), // "sendSkeletonResult"
QT_MOC_LITERAL(5, 73, 7), // "cv::Mat"
QT_MOC_LITERAL(6, 81, 7), // "picture"
QT_MOC_LITERAL(7, 89, 22), // "sendReconstructMessage"
QT_MOC_LITERAL(8, 112, 3), // "msg"
QT_MOC_LITERAL(9, 116, 4) // "code"

    },
    "AbstractReconAlgorithm\0sendCloudFitterResult\0"
    "\0succeed\0sendSkeletonResult\0cv::Mat\0"
    "picture\0sendReconstructMessage\0msg\0"
    "code"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AbstractReconAlgorithm[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    1,   32,    2, 0x06 /* Public */,
       7,    2,   35,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    8,    9,

       0        // eod
};

void AbstractReconAlgorithm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AbstractReconAlgorithm *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendCloudFitterResult((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->sendSkeletonResult((*reinterpret_cast< const cv::Mat(*)>(_a[1]))); break;
        case 2: _t->sendReconstructMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AbstractReconAlgorithm::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractReconAlgorithm::sendCloudFitterResult)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AbstractReconAlgorithm::*)(const cv::Mat & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractReconAlgorithm::sendSkeletonResult)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (AbstractReconAlgorithm::*)(QString , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractReconAlgorithm::sendReconstructMessage)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AbstractReconAlgorithm::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_AbstractReconAlgorithm.data,
    qt_meta_data_AbstractReconAlgorithm,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AbstractReconAlgorithm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AbstractReconAlgorithm::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AbstractReconAlgorithm.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AbstractReconAlgorithm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void AbstractReconAlgorithm::sendCloudFitterResult(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AbstractReconAlgorithm::sendSkeletonResult(const cv::Mat & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AbstractReconAlgorithm::sendReconstructMessage(QString _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
