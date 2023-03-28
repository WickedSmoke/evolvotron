/****************************************************************************
** Meta object code from reading C++ file 'dialog_mutation_parameters.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../dialog_mutation_parameters.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialog_mutation_parameters.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DialogMutationParameters_t {
    QByteArrayData data[19];
    char stringdata0[265];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DialogMutationParameters_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DialogMutationParameters_t qt_meta_stringdata_DialogMutationParameters = {
    {
QT_MOC_LITERAL(0, 0, 24), // "DialogMutationParameters"
QT_MOC_LITERAL(1, 25, 5), // "reset"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 4), // "heat"
QT_MOC_LITERAL(4, 37, 4), // "cool"
QT_MOC_LITERAL(5, 42, 9), // "irradiate"
QT_MOC_LITERAL(6, 52, 6), // "shield"
QT_MOC_LITERAL(7, 59, 6), // "reheat"
QT_MOC_LITERAL(8, 66, 23), // "changed_autocool_enable"
QT_MOC_LITERAL(9, 90, 11), // "buttonstate"
QT_MOC_LITERAL(10, 102, 17), // "changed_magnitude"
QT_MOC_LITERAL(11, 120, 1), // "v"
QT_MOC_LITERAL(12, 122, 23), // "changed_parameter_reset"
QT_MOC_LITERAL(13, 146, 14), // "changed_glitch"
QT_MOC_LITERAL(14, 161, 15), // "changed_shuffle"
QT_MOC_LITERAL(15, 177, 14), // "changed_insert"
QT_MOC_LITERAL(16, 192, 18), // "changed_substitute"
QT_MOC_LITERAL(17, 211, 25), // "changed_autocool_halflife"
QT_MOC_LITERAL(18, 237, 27) // "mutation_parameters_changed"

    },
    "DialogMutationParameters\0reset\0\0heat\0"
    "cool\0irradiate\0shield\0reheat\0"
    "changed_autocool_enable\0buttonstate\0"
    "changed_magnitude\0v\0changed_parameter_reset\0"
    "changed_glitch\0changed_shuffle\0"
    "changed_insert\0changed_substitute\0"
    "changed_autocool_halflife\0"
    "mutation_parameters_changed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DialogMutationParameters[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x0a /* Public */,
       3,    0,   90,    2, 0x0a /* Public */,
       4,    0,   91,    2, 0x0a /* Public */,
       5,    0,   92,    2, 0x0a /* Public */,
       6,    0,   93,    2, 0x0a /* Public */,
       7,    0,   94,    2, 0x0a /* Public */,
       8,    1,   95,    2, 0x0a /* Public */,
      10,    1,   98,    2, 0x0a /* Public */,
      12,    1,  101,    2, 0x0a /* Public */,
      13,    1,  104,    2, 0x0a /* Public */,
      14,    1,  107,    2, 0x0a /* Public */,
      15,    1,  110,    2, 0x0a /* Public */,
      16,    1,  113,    2, 0x0a /* Public */,
      17,    1,  116,    2, 0x0a /* Public */,
      18,    0,  119,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void,

       0        // eod
};

void DialogMutationParameters::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DialogMutationParameters *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->reset(); break;
        case 1: _t->heat(); break;
        case 2: _t->cool(); break;
        case 3: _t->irradiate(); break;
        case 4: _t->shield(); break;
        case 5: _t->reheat(); break;
        case 6: _t->changed_autocool_enable((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->changed_magnitude((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->changed_parameter_reset((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->changed_glitch((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->changed_shuffle((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->changed_insert((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->changed_substitute((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->changed_autocool_halflife((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->mutation_parameters_changed(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DialogMutationParameters::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_DialogMutationParameters.data,
    qt_meta_data_DialogMutationParameters,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DialogMutationParameters::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DialogMutationParameters::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DialogMutationParameters.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int DialogMutationParameters::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
