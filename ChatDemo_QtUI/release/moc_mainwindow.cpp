/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[23];
    char stringdata0[320];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 11), // "sendRequest"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 11), // "onReadyRead"
QT_MOC_LITERAL(4, 36, 18), // "onResponseFinished"
QT_MOC_LITERAL(5, 55, 24), // "on_upLoadFileBtn_clicked"
QT_MOC_LITERAL(6, 80, 18), // "on_StopBtn_clicked"
QT_MOC_LITERAL(7, 99, 19), // "updateAnswerDisplay"
QT_MOC_LITERAL(8, 119, 13), // "appendMessage"
QT_MOC_LITERAL(9, 133, 4), // "role"
QT_MOC_LITERAL(10, 138, 7), // "content"
QT_MOC_LITERAL(11, 146, 14), // "markdownToHtml"
QT_MOC_LITERAL(12, 161, 8), // "markdown"
QT_MOC_LITERAL(13, 170, 15), // "formatCellValue"
QT_MOC_LITERAL(14, 186, 5), // "value"
QT_MOC_LITERAL(15, 192, 15), // "onAnchorClicked"
QT_MOC_LITERAL(16, 208, 4), // "link"
QT_MOC_LITERAL(17, 213, 10), // "playSpeech"
QT_MOC_LITERAL(18, 224, 25), // "on_InterSearchBtn_clicked"
QT_MOC_LITERAL(19, 250, 19), // "on_VoiceBtn_clicked"
QT_MOC_LITERAL(20, 270, 28), // "handleSpeechRecognitionReply"
QT_MOC_LITERAL(21, 299, 14), // "QNetworkReply*"
QT_MOC_LITERAL(22, 314, 5) // "reply"

    },
    "MainWindow\0sendRequest\0\0onReadyRead\0"
    "onResponseFinished\0on_upLoadFileBtn_clicked\0"
    "on_StopBtn_clicked\0updateAnswerDisplay\0"
    "appendMessage\0role\0content\0markdownToHtml\0"
    "markdown\0formatCellValue\0value\0"
    "onAnchorClicked\0link\0playSpeech\0"
    "on_InterSearchBtn_clicked\0on_VoiceBtn_clicked\0"
    "handleSpeechRecognitionReply\0"
    "QNetworkReply*\0reply"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x08 /* Private */,
       3,    0,   85,    2, 0x08 /* Private */,
       4,    0,   86,    2, 0x08 /* Private */,
       5,    0,   87,    2, 0x08 /* Private */,
       6,    0,   88,    2, 0x08 /* Private */,
       7,    0,   89,    2, 0x08 /* Private */,
       8,    2,   90,    2, 0x08 /* Private */,
      11,    1,   95,    2, 0x08 /* Private */,
      13,    1,   98,    2, 0x08 /* Private */,
      15,    1,  101,    2, 0x08 /* Private */,
      17,    0,  104,    2, 0x08 /* Private */,
      18,    0,  105,    2, 0x08 /* Private */,
      19,    0,  106,    2, 0x08 /* Private */,
      20,    1,  107,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    9,   10,
    QMetaType::QString, QMetaType::QString,   12,
    QMetaType::QString, QMetaType::QVariant,   14,
    QMetaType::Void, QMetaType::QUrl,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 21,   22,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendRequest(); break;
        case 1: _t->onReadyRead(); break;
        case 2: _t->onResponseFinished(); break;
        case 3: _t->on_upLoadFileBtn_clicked(); break;
        case 4: _t->on_StopBtn_clicked(); break;
        case 5: _t->updateAnswerDisplay(); break;
        case 6: _t->appendMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 7: { QString _r = _t->markdownToHtml((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 8: { QString _r = _t->formatCellValue((*reinterpret_cast< const QVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 9: _t->onAnchorClicked((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 10: _t->playSpeech(); break;
        case 11: _t->on_InterSearchBtn_clicked(); break;
        case 12: _t->on_VoiceBtn_clicked(); break;
        case 13: _t->handleSpeechRecognitionReply((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
