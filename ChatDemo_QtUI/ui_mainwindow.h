/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTextBrowser *historyDisplay;
    QLineEdit *inputBox;
    QPushButton *sendButton;
    QPushButton *upLoadFileBtn;
    QPushButton *StopBtn;
    QPushButton *exitBtn;
    QPushButton *LogoBtn;
    QPushButton *VoiceBtn;
    QPushButton *InterSearchBtn;
    QTextBrowser *ProcessBrowser;
    QPushButton *SaveBtn;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1396, 981);
        MainWindow->setStyleSheet(QStringLiteral("background-color: rgb(255, 255, 255);"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        historyDisplay = new QTextBrowser(centralWidget);
        historyDisplay->setObjectName(QStringLiteral("historyDisplay"));
        historyDisplay->setGeometry(QRect(40, 110, 891, 611));
        historyDisplay->setStyleSheet(QLatin1String("border:2px solid rgb(183, 199, 221);\n"
"font: 12pt \"Microsoft YaHei UI\";\n"
"border-radius:35px;"));
        inputBox = new QLineEdit(centralWidget);
        inputBox->setObjectName(QStringLiteral("inputBox"));
        inputBox->setGeometry(QRect(130, 830, 1001, 100));
        inputBox->setStyleSheet(QStringLiteral(""));
        sendButton = new QPushButton(centralWidget);
        sendButton->setObjectName(QStringLiteral("sendButton"));
        sendButton->setGeometry(QRect(1170, 830, 100, 100));
        sendButton->setStyleSheet(QStringLiteral(""));
        QIcon icon;
        icon.addFile(QStringLiteral("icons/send.png"), QSize(), QIcon::Normal, QIcon::Off);
        sendButton->setIcon(icon);
        sendButton->setIconSize(QSize(90, 90));
        upLoadFileBtn = new QPushButton(centralWidget);
        upLoadFileBtn->setObjectName(QStringLiteral("upLoadFileBtn"));
        upLoadFileBtn->setGeometry(QRect(20, 730, 80, 80));
        upLoadFileBtn->setInputMethodHints(Qt::ImhNone);
        QIcon icon1;
        icon1.addFile(QStringLiteral("icons/upload.png"), QSize(), QIcon::Normal, QIcon::Off);
        upLoadFileBtn->setIcon(icon1);
        upLoadFileBtn->setIconSize(QSize(70, 70));
        StopBtn = new QPushButton(centralWidget);
        StopBtn->setObjectName(QStringLiteral("StopBtn"));
        StopBtn->setGeometry(QRect(1290, 830, 100, 100));
        QIcon icon2;
        icon2.addFile(QStringLiteral("icons/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        StopBtn->setIcon(icon2);
        StopBtn->setIconSize(QSize(80, 80));
        exitBtn = new QPushButton(centralWidget);
        exitBtn->setObjectName(QStringLiteral("exitBtn"));
        exitBtn->setGeometry(QRect(1300, 10, 90, 90));
        QIcon icon3;
        icon3.addFile(QStringLiteral("icons/close.png"), QSize(), QIcon::Normal, QIcon::Off);
        exitBtn->setIcon(icon3);
        exitBtn->setIconSize(QSize(80, 80));
        LogoBtn = new QPushButton(centralWidget);
        LogoBtn->setObjectName(QStringLiteral("LogoBtn"));
        LogoBtn->setGeometry(QRect(20, 0, 100, 100));
        QIcon icon4;
        icon4.addFile(QStringLiteral("icons/logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        LogoBtn->setIcon(icon4);
        LogoBtn->setIconSize(QSize(100, 100));
        VoiceBtn = new QPushButton(centralWidget);
        VoiceBtn->setObjectName(QStringLiteral("VoiceBtn"));
        VoiceBtn->setGeometry(QRect(129, 730, 80, 80));
        QIcon icon5;
        icon5.addFile(QStringLiteral("icons/voice.png"), QSize(), QIcon::Normal, QIcon::Off);
        VoiceBtn->setIcon(icon5);
        VoiceBtn->setIconSize(QSize(60, 60));
        InterSearchBtn = new QPushButton(centralWidget);
        InterSearchBtn->setObjectName(QStringLiteral("InterSearchBtn"));
        InterSearchBtn->setGeometry(QRect(240, 730, 80, 80));
        QIcon icon6;
        icon6.addFile(QStringLiteral("icons/web.png"), QSize(), QIcon::Normal, QIcon::Off);
        InterSearchBtn->setIcon(icon6);
        InterSearchBtn->setIconSize(QSize(100, 100));
        ProcessBrowser = new QTextBrowser(centralWidget);
        ProcessBrowser->setObjectName(QStringLiteral("ProcessBrowser"));
        ProcessBrowser->setGeometry(QRect(950, 110, 421, 611));
        ProcessBrowser->setStyleSheet(QLatin1String("border:2px solid rgb(183, 199, 221);\n"
"font: 12pt \"Microsoft YaHei UI\";\n"
"border-radius:35px;"));
        SaveBtn = new QPushButton(centralWidget);
        SaveBtn->setObjectName(QStringLiteral("SaveBtn"));
        SaveBtn->setGeometry(QRect(350, 730, 80, 80));
        QIcon icon7;
        icon7.addFile(QStringLiteral("icons/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        SaveBtn->setIcon(icon7);
        SaveBtn->setIconSize(QSize(80, 80));
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
#ifndef QT_NO_TOOLTIP
        historyDisplay->setToolTip(QApplication::translate("MainWindow", "\346\255\244\345\244\204\344\270\272AI\345\257\271\350\257\235\347\225\214\351\235\242", nullptr));
#endif // QT_NO_TOOLTIP
        inputBox->setPlaceholderText(QApplication::translate("MainWindow", "\350\257\267\350\276\223\345\205\245\351\227\256\351\242\230......", nullptr));
#ifndef QT_NO_TOOLTIP
        sendButton->setToolTip(QApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
#endif // QT_NO_TOOLTIP
        sendButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        upLoadFileBtn->setToolTip(QApplication::translate("MainWindow", "\346\226\207\344\273\266\344\270\212\344\274\240", nullptr));
#endif // QT_NO_TOOLTIP
        upLoadFileBtn->setText(QString());
#ifndef QT_NO_TOOLTIP
        StopBtn->setToolTip(QApplication::translate("MainWindow", "\346\232\202\345\201\234\346\200\235\350\200\203", nullptr));
#endif // QT_NO_TOOLTIP
        StopBtn->setText(QString());
        exitBtn->setText(QString());
        LogoBtn->setText(QString());
#ifndef QT_NO_TOOLTIP
        VoiceBtn->setToolTip(QApplication::translate("MainWindow", "\350\257\255\351\237\263\345\217\221\351\200\201", nullptr));
#endif // QT_NO_TOOLTIP
        VoiceBtn->setText(QString());
#ifndef QT_NO_TOOLTIP
        InterSearchBtn->setToolTip(QApplication::translate("MainWindow", "\350\201\224\347\275\221\346\220\234\347\264\242", nullptr));
#endif // QT_NO_TOOLTIP
        InterSearchBtn->setText(QString());
#ifndef QT_NO_TOOLTIP
        ProcessBrowser->setToolTip(QApplication::translate("MainWindow", "\345\257\274\345\205\245Excel\345\220\216\357\274\214\345\244\204\347\220\206\347\232\204\346\225\260\346\215\256\345\260\206\345\234\250\346\255\244\345\244\204\346\230\276\347\244\272", nullptr));
#endif // QT_NO_TOOLTIP
        ProcessBrowser->setDocumentTitle(QApplication::translate("MainWindow", "Data Analyssis", nullptr));
#ifndef QT_NO_TOOLTIP
        SaveBtn->setToolTip(QApplication::translate("MainWindow", "\346\226\207\344\273\266\344\277\235\345\255\230", nullptr));
#endif // QT_NO_TOOLTIP
        SaveBtn->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
