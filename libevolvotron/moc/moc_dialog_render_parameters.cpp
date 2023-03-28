/****************************************************************************
** Meta object code from reading C++ file 'dialog_render_parameters.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../dialog_render_parameters.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialog_render_parameters.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DialogRenderParameters_t {
    QByteArrayData data[7];
    char stringdata0[111];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DialogRenderParameters_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DialogRenderParameters_t qt_meta_stringdata_DialogRenderParameters = {
    {
QT_MOC_LITERAL(0, 0, 22), // "DialogRenderParameters"
QT_MOC_LITERAL(1, 23, 24), // "changed_jittered_samples"
QT_MOC_LITERAL(2, 48, 0), // ""
QT_MOC_LITERAL(3, 49, 11), // "buttonstate"
QT_MOC_LITERAL(4, 61, 20), // "changed_oversampling"
QT_MOC_LITERAL(5, 82, 2), // "id"
QT_MOC_LITERAL(6, 85, 25) // "render_parameters_changed"

    },
    "DialogRenderParameters\0changed_jittered_samples\0"
    "\0buttonstate\0changed_oversampling\0id\0"
    "render_parameters_changed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DialogRenderParameters[] = {

 // content:
       8,       // revision
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
       4,    1,   32,    2, 0x0a /* Public */,
       6,    0,   35,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,

       0        // eod
};

void DialogRenderParameters::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DialogRenderParameters *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->changed_jittered_samples((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->changed_oversampling((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->render_parameters_changed(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DialogRenderParameters::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_DialogRenderParameters.data,
    qt_meta_data_DialogRenderParameters,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DialogRenderParameters::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DialogRenderParameters::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DialogRenderParameters.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int DialogRenderParameters::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
