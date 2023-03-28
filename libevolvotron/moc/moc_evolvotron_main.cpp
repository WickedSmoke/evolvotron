/****************************************************************************
** Meta object code from reading C++ file 'evolvotron_main.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../evolvotron_main.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'evolvotron_main.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SignalExpanderRestartWith_t {
    QByteArrayData data[4];
    char stringdata0[68];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SignalExpanderRestartWith_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SignalExpanderRestartWith_t qt_meta_stringdata_SignalExpanderRestartWith = {
    {
QT_MOC_LITERAL(0, 0, 25), // "SignalExpanderRestartWith"
QT_MOC_LITERAL(1, 26, 12), // "restart_with"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 27) // "const FunctionRegistration*"

    },
    "SignalExpanderRestartWith\0restart_with\0"
    "\0const FunctionRegistration*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SignalExpanderRestartWith[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       1,    0,   27,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void SignalExpanderRestartWith::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SignalExpanderRestartWith *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->restart_with((*reinterpret_cast< const FunctionRegistration*(*)>(_a[1]))); break;
        case 1: _t->restart_with(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SignalExpanderRestartWith::*)(const FunctionRegistration * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SignalExpanderRestartWith::restart_with)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SignalExpanderRestartWith::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SignalExpanderRestartWith.data,
    qt_meta_data_SignalExpanderRestartWith,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SignalExpanderRestartWith::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SignalExpanderRestartWith::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SignalExpanderRestartWith.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SignalExpanderRestartWith::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void SignalExpanderRestartWith::restart_with(const FunctionRegistration * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_EvolvotronMain_t {
    QByteArrayData data[15];
    char stringdata0[217];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EvolvotronMain_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EvolvotronMain_t qt_meta_stringdata_EvolvotronMain = {
    {
QT_MOC_LITERAL(0, 0, 14), // "EvolvotronMain"
QT_MOC_LITERAL(1, 15, 4), // "tick"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 4), // "undo"
QT_MOC_LITERAL(4, 26, 18), // "simplify_constants"
QT_MOC_LITERAL(5, 45, 16), // "toggle_hide_menu"
QT_MOC_LITERAL(6, 62, 17), // "toggle_fullscreen"
QT_MOC_LITERAL(7, 80, 5), // "reset"
QT_MOC_LITERAL(8, 86, 25), // "reset_mutation_parameters"
QT_MOC_LITERAL(9, 112, 11), // "reset_locks"
QT_MOC_LITERAL(10, 124, 10), // "reset_warm"
QT_MOC_LITERAL(11, 135, 10), // "reset_cold"
QT_MOC_LITERAL(12, 146, 16), // "reset_randomized"
QT_MOC_LITERAL(13, 163, 27), // "mutation_parameters_changed"
QT_MOC_LITERAL(14, 191, 25) // "render_parameters_changed"

    },
    "EvolvotronMain\0tick\0\0undo\0simplify_constants\0"
    "toggle_hide_menu\0toggle_fullscreen\0"
    "reset\0reset_mutation_parameters\0"
    "reset_locks\0reset_warm\0reset_cold\0"
    "reset_randomized\0mutation_parameters_changed\0"
    "render_parameters_changed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EvolvotronMain[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x09 /* Protected */,
       3,    0,   70,    2, 0x09 /* Protected */,
       4,    0,   71,    2, 0x09 /* Protected */,
       5,    0,   72,    2, 0x0a /* Public */,
       6,    0,   73,    2, 0x0a /* Public */,
       7,    2,   74,    2, 0x0a /* Public */,
      10,    0,   79,    2, 0x0a /* Public */,
      11,    0,   80,    2, 0x0a /* Public */,
      12,    0,   81,    2, 0x0a /* Public */,
      13,    0,   82,    2, 0x0a /* Public */,
      14,    0,   83,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,    8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void EvolvotronMain::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<EvolvotronMain *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->tick(); break;
        case 1: _t->undo(); break;
        case 2: _t->simplify_constants(); break;
        case 3: _t->toggle_hide_menu(); break;
        case 4: _t->toggle_fullscreen(); break;
        case 5: _t->reset((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 6: _t->reset_warm(); break;
        case 7: _t->reset_cold(); break;
        case 8: _t->reset_randomized(); break;
        case 9: _t->mutation_parameters_changed(); break;
        case 10: _t->render_parameters_changed(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject EvolvotronMain::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_EvolvotronMain.data,
    qt_meta_data_EvolvotronMain,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *EvolvotronMain::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EvolvotronMain::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EvolvotronMain.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int EvolvotronMain::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
