/****************************************************************************
** Meta object code from reading C++ file 'encoderstream.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../app/encoderstream.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'encoderstream.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_EncoderStream_t {
    uint offsetsAndSizes[38];
    char stringdata0[14];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[20];
    char stringdata4[9];
    char stringdata5[19];
    char stringdata6[8];
    char stringdata7[9];
    char stringdata8[14];
    char stringdata9[4];
    char stringdata10[18];
    char stringdata11[16];
    char stringdata12[15];
    char stringdata13[6];
    char stringdata14[20];
    char stringdata15[7];
    char stringdata16[9];
    char stringdata17[10];
    char stringdata18[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_EncoderStream_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_EncoderStream_t qt_meta_stringdata_EncoderStream = {
    {
        QT_MOC_LITERAL(0, 13),  // "EncoderStream"
        QT_MOC_LITERAL(14, 17),  // "onEncodingStarted"
        QT_MOC_LITERAL(32, 0),  // ""
        QT_MOC_LITERAL(33, 19),  // "onEncodingInitError"
        QT_MOC_LITERAL(53, 8),  // "_message"
        QT_MOC_LITERAL(62, 18),  // "onEncodingProgress"
        QT_MOC_LITERAL(81, 7),  // "percent"
        QT_MOC_LITERAL(89, 8),  // "rem_time"
        QT_MOC_LITERAL(98, 13),  // "onEncodingLog"
        QT_MOC_LITERAL(112, 3),  // "log"
        QT_MOC_LITERAL(116, 17),  // "onEncodingAborted"
        QT_MOC_LITERAL(134, 15),  // "onEncodingError"
        QT_MOC_LITERAL(150, 14),  // "_error_message"
        QT_MOC_LITERAL(165, 5),  // "popup"
        QT_MOC_LITERAL(171, 19),  // "onEncodingCompleted"
        QT_MOC_LITERAL(191, 6),  // "encode"
        QT_MOC_LITERAL(198, 8),  // "progress"
        QT_MOC_LITERAL(207, 9),  // "completed"
        QT_MOC_LITERAL(217, 5)   // "abort"
    },
    "EncoderStream",
    "onEncodingStarted",
    "",
    "onEncodingInitError",
    "_message",
    "onEncodingProgress",
    "percent",
    "rem_time",
    "onEncodingLog",
    "log",
    "onEncodingAborted",
    "onEncodingError",
    "_error_message",
    "popup",
    "onEncodingCompleted",
    "encode",
    "progress",
    "completed",
    "abort"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_EncoderStream[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   86,    2, 0x06,    1 /* Public */,
       3,    1,   87,    2, 0x06,    2 /* Public */,
       5,    2,   90,    2, 0x06,    4 /* Public */,
       8,    1,   95,    2, 0x06,    7 /* Public */,
      10,    0,   98,    2, 0x06,    9 /* Public */,
      11,    2,   99,    2, 0x06,   10 /* Public */,
      11,    1,  104,    2, 0x26,   13 /* Public | MethodCloned */,
      14,    0,  107,    2, 0x06,   15 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      15,    0,  108,    2, 0x08,   16 /* Private */,
      16,    0,  109,    2, 0x08,   17 /* Private */,
      17,    1,  110,    2, 0x08,   18 /* Private */,
      18,    0,  113,    2, 0x08,   20 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Float,    6,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   12,   13,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject EncoderStream::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_EncoderStream.offsetsAndSizes,
    qt_meta_data_EncoderStream,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_EncoderStream_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<EncoderStream, std::true_type>,
        // method 'onEncodingStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEncodingInitError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onEncodingProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'onEncodingLog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onEncodingAborted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEncodingError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onEncodingError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onEncodingCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'encode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'progress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'completed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'abort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void EncoderStream::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<EncoderStream *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onEncodingStarted(); break;
        case 1: _t->onEncodingInitError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->onEncodingProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 3: _t->onEncodingLog((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->onEncodingAborted(); break;
        case 5: _t->onEncodingError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 6: _t->onEncodingError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onEncodingCompleted(); break;
        case 8: _t->encode(); break;
        case 9: _t->progress(); break;
        case 10: _t->completed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->abort(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (EncoderStream::*)();
            if (_t _q_method = &EncoderStream::onEncodingStarted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (EncoderStream::*)(const QString & );
            if (_t _q_method = &EncoderStream::onEncodingInitError; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (EncoderStream::*)(int , float );
            if (_t _q_method = &EncoderStream::onEncodingProgress; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (EncoderStream::*)(const QString & );
            if (_t _q_method = &EncoderStream::onEncodingLog; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (EncoderStream::*)();
            if (_t _q_method = &EncoderStream::onEncodingAborted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (EncoderStream::*)(const QString & , bool );
            if (_t _q_method = &EncoderStream::onEncodingError; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (EncoderStream::*)();
            if (_t _q_method = &EncoderStream::onEncodingCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject *EncoderStream::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EncoderStream::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EncoderStream.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int EncoderStream::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void EncoderStream::onEncodingStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void EncoderStream::onEncodingInitError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void EncoderStream::onEncodingProgress(int _t1, float _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void EncoderStream::onEncodingLog(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void EncoderStream::onEncodingAborted()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void EncoderStream::onEncodingError(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 7
void EncoderStream::onEncodingCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
