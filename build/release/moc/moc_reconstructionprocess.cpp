/****************************************************************************
** Meta object code from reading C++ file 'reconstructionprocess.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/reconstructionProcess/reconstructionprocess.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'reconstructionprocess.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ReconstructionProcess_t {
    QByteArrayData data[12];
    char stringdata0[155];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ReconstructionProcess_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ReconstructionProcess_t qt_meta_stringdata_ReconstructionProcess = {
    {
QT_MOC_LITERAL(0, 0, 21), // "ReconstructionProcess"
QT_MOC_LITERAL(1, 22, 21), // "sendCloudFitterResult"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 7), // "succeed"
QT_MOC_LITERAL(4, 53, 18), // "sendSkeletonResult"
QT_MOC_LITERAL(5, 72, 7), // "cv::Mat"
QT_MOC_LITERAL(6, 80, 7), // "picture"
QT_MOC_LITERAL(7, 88, 24), // "sendReconstructionResult"
QT_MOC_LITERAL(8, 113, 25), // "sendReconstructionMessage"
QT_MOC_LITERAL(9, 139, 6), // "module"
QT_MOC_LITERAL(10, 146, 3), // "msg"
QT_MOC_LITERAL(11, 150, 4) // "code"

    },
    "ReconstructionProcess\0sendCloudFitterResult\0"
    "\0succeed\0sendSkeletonResult\0cv::Mat\0"
    "picture\0sendReconstructionResult\0"
    "sendReconstructionMessage\0module\0msg\0"
    "code"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ReconstructionProcess[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,
       7,    1,   40,    2, 0x06 /* Public */,
       8,    3,   43,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Int,    9,   10,   11,

       0        // eod
};

void ReconstructionProcess::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ReconstructionProcess *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendCloudFitterResult((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->sendSkeletonResult((*reinterpret_cast< const cv::Mat(*)>(_a[1]))); break;
        case 2: _t->sendReconstructionResult((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->sendReconstructionMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ReconstructionProcess::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ReconstructionProcess::sendCloudFitterResult)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ReconstructionProcess::*)(const cv::Mat & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ReconstructionProcess::sendSkeletonResult)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ReconstructionProcess::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ReconstructionProcess::sendReconstructionResult)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ReconstructionProcess::*)(QString , QString , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ReconstructionProcess::sendReconstructionMessage)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ReconstructionProcess::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ReconstructionProcess.data,
    qt_meta_data_ReconstructionProcess,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ReconstructionProcess::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ReconstructionProcess::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ReconstructionProcess.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ReconstructionProcess::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ReconstructionProcess::sendCloudFitterResult(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ReconstructionProcess::sendSkeletonResult(const cv::Mat & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ReconstructionProcess::sendReconstructionResult(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ReconstructionProcess::sendReconstructionMessage(QString _t1, QString _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
