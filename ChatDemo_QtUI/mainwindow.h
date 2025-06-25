#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QTextCodec>
#include <QTextToSpeech>
#include <QMouseEvent>
#include <QLine>
#include <QLabel>
#include <QJsonArray>

//窗体圆角化
#include <QBitmap>
#include <QPainter>
#include <QMovie>

//语音识别
#include <QAudioRecorder>
#include <QAudioEncoderSettings>
#include <QDir>
#include <QUrl>
#include <QHttpMultiPart>
#include <QUuid>
#include <QAudioDeviceInfo>
#include <QTimer>
#include <QElapsedTimer>

//添加PDF支持
//#include <poppler-qt5.h>
#include <QProcess>

#include <QAxObject>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void sendRequest();
    void onReadyRead();
    void onResponseFinished();
    void on_upLoadFileBtn_clicked();

    void on_StopBtn_clicked();
    void updateAnswerDisplay();
    void appendMessage(const QString& role, const QString& content);
    QString markdownToHtml(const QString& markdown);
    QString formatCellValue(const QVariant& value);
    void onAnchorClicked(const QUrl &link); // Add this line
    void playSpeech();
    void on_InterSearchBtn_clicked();

    void on_VoiceBtn_clicked();
//    void handleTokenReply(QNetworkReply* reply);
    void handleSpeechRecognitionReply(QNetworkReply* reply);

private:
    //窗体可拖动
     void mouseMoveEvent(QMouseEvent *event);
     void mousePressEvent(QMouseEvent *event);
     void mouseReleaseEvent(QMouseEvent *event);
     QPoint z;
     void mouseDoubleClickEvent(QMouseEvent *event);
     QString excelToMarkdown(const QString& fileName);
     bool isExcelAvailable();
     QList<QString> uploadedFiles; // 存储已上传文件路径
     QMap<QString, QString> fileContents; // 文件路径->处理后的内容
     void displayAIResponse(const QString &answer);
     void sendRequest_Mutil();

private:
    QString baiduAccessToken="24.c4272d1566506b49bd783608debe2fc0.2592000.1742723734.282335-117663972";
    QNetworkAccessManager* tokenManager;
    QAudioRecorder* audioRecorder;
    QFile outputFile;
    void getBaiduToken();
    QString deviceID;
    QElapsedTimer recordTimer;
    QTimer *durationTimer;
    QString readExcelFile(const QString& filePath);
    QChar detectDelimiter(const QString& sampleLine);
    void appendErrorMessage(const QString& message);
    void appendSuccessMessage(const QString& message);
    void appendMarkdownContent(const QString& content);
    QString readCSVFile(const QString& filePath);

private:
    void BeaBtn(); //按钮的美化
    void Beawidget(); //窗口界面的优化
    void processAudioFile();

private:
    // 成员声明顺序与初始化列表一致
    Ui::MainWindow *ui;
    QTextToSpeech* speech;          // 声明在前
    QNetworkAccessManager* networkManager;
    QNetworkReply* currentReply;
    int currentAICursorPosition;
    QString currentAIAnswer;
    QString fileContent;
    bool isAIAnswerComplete = false;  // 添加状态标志
    QString processFile(const QString& filePath); // 文件处理声明
    QDateTime recordStartTime;
    bool debugOutput; // 声明为类的成员变量
    void handleBackendResponse(QNetworkReply* reply);
    void sendJsonToBackend(const QString& jsonData);
    void sendCombinedRequest(const QString& question, const QString& fileData);
    void handleCombinedResponse(QNetworkReply* reply);
    QStringList parseCSVLine(const QString &line, QChar delimiter);
};

#endif // MAINWINDOW_H
