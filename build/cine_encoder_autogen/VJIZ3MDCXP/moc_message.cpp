/****************************************************************************
** Meta object code from reading C++ file 'message.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../app/message.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'message.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_Message_t {
    uint offsetsAndSizes[12];
    char stringdata0[8];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[14];
    char stringdata4[10];
    char stringdata5[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_Message_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_Message_t qt_meta_stringdata_Message = {
    {
        QT_MOC_LITERAL(0, 7),  // "Message"
        QT_MOC_LITERAL(8, 13),  // "onButtonApply"
        QT_MOC_LITERAL(22, 0),  // ""
        QT_MOC_LITERAL(23, 13),  // "onCloseWindow"
        QT_MOC_LITERAL(37, 9),  // "showEvent"
        QT_MOC_LITERAL(47, 11)   // "QShowEvent*"
    },
    "Message",
    "onButtonApply",
    "",
    "onCloseWindow",
    "showEvent",
    "QShowEvent*"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_Message[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x08,    1 /* Private */,
       3,    0,   33,    2, 0x08,    2 /* Private */,
       4,    1,   34,    2, 0x08,    3 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    2,

       0        // eod
};

Q_CONSTINIT const QMetaObject Message::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseWindow::staticMetaObject>(),
    qt_meta_stringdata_Message.offsetsAndSizes,
    qt_meta_data_Message,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_Message_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Message, std::true_type>,
        // method 'onButtonApply'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCloseWindow'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QShowEvent *, std::false_type>
    >,
    nullptr
} };

void Message::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Message *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onButtonApply(); break;
        case 1: _t->onCloseWindow(); break;
        case 2: _t->showEvent((*reinterpret_cast< std::add_pointer_t<QShowEvent*>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *Message::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Message::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Message.stringdata0))
        return static_cast<void*>(this);
    return BaseWindow::qt_metacast(_clname);
}

int Message::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
