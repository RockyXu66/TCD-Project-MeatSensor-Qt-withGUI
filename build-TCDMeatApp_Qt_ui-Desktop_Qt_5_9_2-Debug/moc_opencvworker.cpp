/****************************************************************************
** Meta object code from reading C++ file 'opencvworker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../TCDMeatApp_Qt_ui/opencvworker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'opencvworker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_OpenCvWorker_t {
    QByteArrayData data[12];
    char stringdata0[144];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OpenCvWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OpenCvWorker_t qt_meta_stringdata_OpenCvWorker = {
    {
QT_MOC_LITERAL(0, 0, 12), // "OpenCvWorker"
QT_MOC_LITERAL(1, 13, 9), // "sendFrame"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 14), // "frameProcessed"
QT_MOC_LITERAL(4, 39, 17), // "sendVideoFinished"
QT_MOC_LITERAL(5, 57, 11), // "sendO2Value"
QT_MOC_LITERAL(6, 69, 7), // "content"
QT_MOC_LITERAL(7, 77, 10), // "sendPrompt"
QT_MOC_LITERAL(8, 88, 18), // "isOxygenCalculated"
QT_MOC_LITERAL(9, 107, 15), // "receiveLeftArea"
QT_MOC_LITERAL(10, 123, 3), // "num"
QT_MOC_LITERAL(11, 127, 16) // "receiveRightArea"

    },
    "OpenCvWorker\0sendFrame\0\0frameProcessed\0"
    "sendVideoFinished\0sendO2Value\0content\0"
    "sendPrompt\0isOxygenCalculated\0"
    "receiveLeftArea\0num\0receiveRightArea"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OpenCvWorker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    0,   47,    2, 0x06 /* Public */,
       5,    1,   48,    2, 0x06 /* Public */,
       7,    1,   51,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    1,   54,    2, 0x08 /* Private */,
      11,    1,   57,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QImage,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::Bool,    8,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::Int,   10,

       0        // eod
};

void OpenCvWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OpenCvWorker *_t = static_cast<OpenCvWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendFrame((*reinterpret_cast< const QImage(*)>(_a[1]))); break;
        case 1: _t->sendVideoFinished(); break;
        case 2: _t->sendO2Value((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->sendPrompt((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->receiveLeftArea((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->receiveRightArea((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (OpenCvWorker::*_t)(const QImage & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OpenCvWorker::sendFrame)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (OpenCvWorker::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OpenCvWorker::sendVideoFinished)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (OpenCvWorker::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OpenCvWorker::sendO2Value)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (OpenCvWorker::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OpenCvWorker::sendPrompt)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject OpenCvWorker::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_OpenCvWorker.data,
      qt_meta_data_OpenCvWorker,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OpenCvWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OpenCvWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OpenCvWorker.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int OpenCvWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
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
void OpenCvWorker::sendFrame(const QImage & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void OpenCvWorker::sendVideoFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void OpenCvWorker::sendO2Value(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void OpenCvWorker::sendPrompt(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
