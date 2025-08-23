/****************************************************************************
** Meta object code from reading C++ file 'streamconverter.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../app/streamconverter.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'streamconverter.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_StreamConverter_t {
    uint offsetsAndSizes[40];
    char stringdata0[16];
    char stringdata1[8];
    char stringdata2[1];
    char stringdata3[7];
    char stringdata4[13];
    char stringdata5[18];
    char stringdata6[20];
    char stringdata7[9];
    char stringdata8[19];
    char stringdata9[8];
    char stringdata10[9];
    char stringdata11[14];
    char stringdata12[4];
    char stringdata13[20];
    char stringdata14[18];
    char stringdata15[16];
    char stringdata16[14];
    char stringdata17[6];
    char stringdata18[6];
    char stringdata19[7];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_StreamConverter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_StreamConverter_t qt_meta_stringdata_StreamConverter = {
    {
        QT_MOC_LITERAL(0, 15),  // "StreamConverter"
        QT_MOC_LITERAL(16, 7),  // "onStart"
        QT_MOC_LITERAL(24, 0),  // ""
        QT_MOC_LITERAL(25, 6),  // "onStop"
        QT_MOC_LITERAL(32, 12),  // "initEncoding"
        QT_MOC_LITERAL(45, 17),  // "onEncodingStarted"
        QT_MOC_LITERAL(63, 19),  // "onEncodingInitError"
        QT_MOC_LITERAL(83, 8),  // "_message"
        QT_MOC_LITERAL(92, 18),  // "onEncodingProgress"
        QT_MOC_LITERAL(111, 7),  // "percent"
        QT_MOC_LITERAL(119, 8),  // "rem_time"
        QT_MOC_LITERAL(128, 13),  // "onEncodingLog"
        QT_MOC_LITERAL(142, 3),  // "log"
        QT_MOC_LITERAL(146, 19),  // "onEncodingCompleted"
        QT_MOC_LITERAL(166, 17),  // "onEncodingAborted"
        QT_MOC_LITERAL(184, 15),  // "onEncodingError"
        QT_MOC_LITERAL(200, 13),  // "error_message"
        QT_MOC_LITERAL(214, 5),  // "popup"
        QT_MOC_LITERAL(220, 5),  // "pause"
        QT_MOC_LITERAL(226, 6)   // "resume"
    },
    "StreamConverter",
    "onStart",
    "",
    "onStop",
    "initEncoding",
    "onEncodingStarted",
    "onEncodingInitError",
    "_message",
    "onEncodingProgress",
    "percent",
    "rem_time",
    "onEncodingLog",
    "log",
    "onEncodingCompleted",
    "onEncodingAborted",
    "onEncodingError",
    "error_message",
    "popup",
    "pause",
    "resume"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_StreamConverter[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x08,    1 /* Private */,
       3,    0,   93,    2, 0x08,    2 /* Private */,
       4,    0,   94,    2, 0x08,    3 /* Private */,
       5,    0,   95,    2, 0x08,    4 /* Private */,
       6,    1,   96,    2, 0x08,    5 /* Private */,
       8,    2,   99,    2, 0x08,    7 /* Private */,
      11,    1,  104,    2, 0x08,   10 /* Private */,
      13,    0,  107,    2, 0x08,   12 /* Private */,
      14,    0,  108,    2, 0x08,   13 /* Private */,
      15,    2,  109,    2, 0x08,   14 /* Private */,
      15,    1,  114,    2, 0x28,   17 /* Private | MethodCloned */,
      18,    0,  117,    2, 0x08,   19 /* Private */,
      19,    0,  118,    2, 0x08,   20 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::Float,    9,   10,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   16,   17,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject StreamConverter::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseWindow::staticMetaObject>(),
    qt_meta_stringdata_StreamConverter.offsetsAndSizes,
    qt_meta_data_StreamConverter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_StreamConverter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<StreamConverter, std::true_type>,
        // method 'onStart'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'initEncoding'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
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
        // method 'onEncodingCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEncodingAborted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEncodingError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onEncodingError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'pause'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'resume'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void StreamConverter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<StreamConverter *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onStart(); break;
        case 1: _t->onStop(); break;
        case 2: _t->initEncoding(); break;
        case 3: _t->onEncodingStarted(); break;
        case 4: _t->onEncodingInitError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onEncodingProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 6: _t->onEncodingLog((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onEncodingCompleted(); break;
        case 8: _t->onEncodingAborted(); break;
        case 9: _t->onEncodingError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 10: _t->onEncodingError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->pause(); break;
        case 12: _t->resume(); break;
        default: ;
        }
    }
}

const QMetaObject *StreamConverter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StreamConverter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_StreamConverter.stringdata0))
        return static_cast<void*>(this);
    return BaseWindow::qt_metacast(_clname);
}

int StreamConverter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
