/****************************************************************************
** Meta object code from reading C++ file 'advancedsetupdialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../src/app/src/advancedsetupdialog.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'advancedsetupdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.1. It"
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
struct qt_meta_stringdata_AdvancedSetupDialog_t {
    uint offsetsAndSizes[44];
    char stringdata0[20];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[8];
    char stringdata5[10];
    char stringdata6[24];
    char stringdata7[8];
    char stringdata8[12];
    char stringdata9[47];
    char stringdata10[7];
    char stringdata11[23];
    char stringdata12[20];
    char stringdata13[36];
    char stringdata14[5];
    char stringdata15[40];
    char stringdata16[38];
    char stringdata17[45];
    char stringdata18[6];
    char stringdata19[40];
    char stringdata20[37];
    char stringdata21[36];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_AdvancedSetupDialog_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_AdvancedSetupDialog_t qt_meta_stringdata_AdvancedSetupDialog = {
    {
        QT_MOC_LITERAL(0, 19),  // "AdvancedSetupDialog"
        QT_MOC_LITERAL(20, 17),  // "sig_ni_dev_change"
        QT_MOC_LITERAL(38, 0),  // ""
        QT_MOC_LITERAL(39, 11),  // "std::string"
        QT_MOC_LITERAL(51, 7),  // "m_niDev"
        QT_MOC_LITERAL(59, 9),  // "m_trigDev"
        QT_MOC_LITERAL(69, 23),  // "sig_trigger_mode_change"
        QT_MOC_LITERAL(93, 7),  // "int16_t"
        QT_MOC_LITERAL(101, 11),  // "triggerMode"
        QT_MOC_LITERAL(113, 46),  // "sig_enable_live_view_during_a..."
        QT_MOC_LITERAL(160, 6),  // "enable"
        QT_MOC_LITERAL(167, 22),  // "sig_close_adv_settings"
        QT_MOC_LITERAL(190, 19),  // "updateAdvancedSetup"
        QT_MOC_LITERAL(210, 35),  // "on_ledDeviceList_currentTextC..."
        QT_MOC_LITERAL(246, 4),  // "text"
        QT_MOC_LITERAL(251, 39),  // "on_triggerDeviceList_currentT..."
        QT_MOC_LITERAL(291, 37),  // "on_triggerModeList_currentTex..."
        QT_MOC_LITERAL(329, 44),  // "on_checkEnableLiveViewDuringA..."
        QT_MOC_LITERAL(374, 5),  // "state"
        QT_MOC_LITERAL(380, 39),  // "on_checkDownsampleRawFiles_st..."
        QT_MOC_LITERAL(420, 36),  // "on_checkKeepOriginalRaw_state..."
        QT_MOC_LITERAL(457, 35)   // "on_binFactorList_currentTextC..."
    },
    "AdvancedSetupDialog",
    "sig_ni_dev_change",
    "",
    "std::string",
    "m_niDev",
    "m_trigDev",
    "sig_trigger_mode_change",
    "int16_t",
    "triggerMode",
    "sig_enable_live_view_during_acquisition_change",
    "enable",
    "sig_close_adv_settings",
    "updateAdvancedSetup",
    "on_ledDeviceList_currentTextChanged",
    "text",
    "on_triggerDeviceList_currentTextChanged",
    "on_triggerModeList_currentTextChanged",
    "on_checkEnableLiveViewDuringAcq_stateChanged",
    "state",
    "on_checkDownsampleRawFiles_stateChanged",
    "on_checkKeepOriginalRaw_stateChanged",
    "on_binFactorList_currentTextChanged"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_AdvancedSetupDialog[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   86,    2, 0x06,    1 /* Public */,
       6,    1,   91,    2, 0x06,    4 /* Public */,
       9,    1,   94,    2, 0x06,    6 /* Public */,
      11,    0,   97,    2, 0x06,    8 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    0,   98,    2, 0x08,    9 /* Private */,
      13,    1,   99,    2, 0x08,   10 /* Private */,
      15,    1,  102,    2, 0x08,   12 /* Private */,
      16,    1,  105,    2, 0x08,   14 /* Private */,
      17,    1,  108,    2, 0x08,   16 /* Private */,
      19,    1,  111,    2, 0x08,   18 /* Private */,
      20,    1,  114,    2, 0x08,   20 /* Private */,
      21,    1,  117,    2, 0x08,   22 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void, QMetaType::QString,   14,

       0        // eod
};

Q_CONSTINIT const QMetaObject AdvancedSetupDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_AdvancedSetupDialog.offsetsAndSizes,
    qt_meta_data_AdvancedSetupDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_AdvancedSetupDialog_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<AdvancedSetupDialog, std::true_type>,
        // method 'sig_ni_dev_change'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::string, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::string, std::false_type>,
        // method 'sig_trigger_mode_change'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int16_t, std::false_type>,
        // method 'sig_enable_live_view_during_acquisition_change'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'sig_close_adv_settings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateAdvancedSetup'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_ledDeviceList_currentTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_triggerDeviceList_currentTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_triggerModeList_currentTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_checkEnableLiveViewDuringAcq_stateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_checkDownsampleRawFiles_stateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_checkKeepOriginalRaw_stateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_binFactorList_currentTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void AdvancedSetupDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AdvancedSetupDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sig_ni_dev_change((*reinterpret_cast< std::add_pointer_t<std::string>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::string>>(_a[2]))); break;
        case 1: _t->sig_trigger_mode_change((*reinterpret_cast< std::add_pointer_t<int16_t>>(_a[1]))); break;
        case 2: _t->sig_enable_live_view_during_acquisition_change((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->sig_close_adv_settings(); break;
        case 4: _t->updateAdvancedSetup(); break;
        case 5: _t->on_ledDeviceList_currentTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->on_triggerDeviceList_currentTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->on_triggerModeList_currentTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->on_checkEnableLiveViewDuringAcq_stateChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->on_checkDownsampleRawFiles_stateChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->on_checkKeepOriginalRaw_stateChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->on_binFactorList_currentTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AdvancedSetupDialog::*)(std::string , std::string );
            if (_t _q_method = &AdvancedSetupDialog::sig_ni_dev_change; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AdvancedSetupDialog::*)(int16_t );
            if (_t _q_method = &AdvancedSetupDialog::sig_trigger_mode_change; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (AdvancedSetupDialog::*)(bool );
            if (_t _q_method = &AdvancedSetupDialog::sig_enable_live_view_during_acquisition_change; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (AdvancedSetupDialog::*)();
            if (_t _q_method = &AdvancedSetupDialog::sig_close_adv_settings; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *AdvancedSetupDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AdvancedSetupDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AdvancedSetupDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int AdvancedSetupDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
void AdvancedSetupDialog::sig_ni_dev_change(std::string _t1, std::string _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AdvancedSetupDialog::sig_trigger_mode_change(int16_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AdvancedSetupDialog::sig_enable_live_view_during_acquisition_change(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void AdvancedSetupDialog::sig_close_adv_settings()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
