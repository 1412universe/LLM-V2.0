#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QDebug>
#include <QRegularExpression>
#include <QScrollBar>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QAxObject>
#include <QTextToSpeech>
#include <QNetworkAccessManager>
#include <QTextBrowser>
#include <QAudioFormat>
#include <QAudioRecorder>
#include <QUrlQuery>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    speech(new QTextToSpeech(this)),
    networkManager(new QNetworkAccessManager(this)),
    currentReply(nullptr),
    currentAICursorPosition(-1),
    tokenManager(new QNetworkAccessManager(this)),
    audioRecorder(new QAudioRecorder(this))
{
    ui->setupUi(this);
    setWindowTitle("AI 助手");

    speech->setLocale(QLocale::Chinese);
    speech->setRate(0.3);
    speech->setPitch(0.7);
    speech->setVolume(1.0);

    connect(ui->inputBox, &QLineEdit::returnPressed, this, &MainWindow::sendRequest_Mutil);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::sendRequest_Mutil);
    connect(ui->historyDisplay, &QTextBrowser::anchorClicked, this, &MainWindow::onAnchorClicked);

    BeaBtn(); //按钮的美化
    Beawidget(); //窗口界面的优化
    // 生成设备唯一标识
    deviceID = QUuid::createUuid().toString().remove("{").remove("}");

    // 添加录音设置
     QAudioEncoderSettings audioSettings;
     audioSettings.setCodec("audio/pcm");
     audioSettings.setQuality(QMultimedia::HighQuality);
     audioSettings.setSampleRate(16000);
     audioSettings.setChannelCount(1);
     audioSettings.setEncodingMode(QMultimedia::ConstantQualityEncoding);
     audioRecorder->setAudioSettings(audioSettings);
     durationTimer = new QTimer(this);
     connect(durationTimer, &QTimer::timeout, this, [this]() {
            if (audioRecorder->state() == QAudioRecorder::RecordingState) {
                qint64 durationMs = recordTimer.elapsed();
                ui->VoiceBtn->setText(QString("停止录音 (%1s)").arg(durationMs / 1000));
            }
        });
     connect(audioRecorder, QOverload<QMediaRecorder::Error>::of(&QAudioRecorder::error),
            [this](QMediaRecorder::Error error) {
                QMessageBox::warning(this, "录音错误", audioRecorder->errorString());
      });

     // 获取百度token
     getBaiduToken();


}


MainWindow::~MainWindow()
{
    delete ui;
    // 遵循RAII原则，智能指针会自动释放
    // 移除显式delete speech操作
}
void MainWindow::displayAIResponse(const QString &answer)
{
    QTextCursor cursor(ui->historyDisplay->document());
    cursor.setPosition(currentAICursorPosition);

    // 移除加载动画
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    // 构建带格式的AI回答
    QString formattedAnswer = QString(R"(
        <div class="message ai-message">
            <div class="ai-header">
                <img class="avatar ai-avatar" src=":/icons/ai.png">
                <span class="ai-title">AI助手</span>
            </div>
            <div class="message-content">
                %1
            </div>
            <div class="message-footer">
                %2
                <a href="play://speech" class="play-btn">▶︎ 播放</a>
            </div>
        </div>
    )").arg(markdownToHtml(answer))
     .arg(QDateTime::currentDateTime().toString("HH:mm"));

    cursor.insertHtml(formattedAnswer);

    // 自动滚动到底部
    QScrollBar* scrollBar = ui->historyDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}


// 修改后的发送请求函数
void MainWindow::sendRequest_Mutil()
{
    QString question = ui->inputBox->text().trimmed();
    if (question.isEmpty()) {
        appendErrorMessage("问题内容不能为空");
        return;
    }
    // 用户消息
    QString userMessage = QString("<div class='message user-message'>"
                                  "<img class='avatar user-avatar' src=':/icons/user.png' alt='User Avatar'>"
                                  "<div class='message-content'>%1</div>"
                                  "</div>")
                          .arg(markdownToHtml("**用户**：" + question));
    ui->historyDisplay->append(userMessage);

    // AI 消息
    QString aiMessage = QString("<div class='message ai-message'>"
                                "<div class='message-content'>%1</div>"
                                "<img class='avatar ai-avatar' src=':/icons/loading.png' alt='AI Avatar'>"
                                "</div>")
                        .arg(markdownToHtml("**AI助手**： "));

   if (question.isEmpty()) {
            ui->historyDisplay->append(markdownToHtml("**错误**：问题不能为空！"));
            return;
        }
    ui->inputBox->clear();
    ui->historyDisplay->append(aiMessage);
    currentAIAnswer = "";
    // 添加用户消息（强制换行）
//    appendMessage("user", question);

    // 添加AI等待提示（独立消息块）
//    appendMessage("ai", "正在思考...");
    isAIAnswerComplete = false;
    QTextCursor cursor = ui->historyDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1);
    currentAICursorPosition = cursor.position();


    // 构建请求数据
    QJsonObject requestData;
    requestData["question"] = question;

    // 添加文件内容（如果有）
    if (!fileContent.isEmpty()) {
        requestData["file_content"] = fileContent;
        fileContent.clear();
    }

    // 发送请求
    QNetworkRequest request(QUrl("http://127.0.0.1:8080/api/chat/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (currentReply) {
        currentReply->abort();
        currentReply->deleteLater();
    }

    currentReply = networkManager->post(request, QJsonDocument(requestData).toJson());
    connect(currentReply, &QNetworkReply::readyRead, this, &MainWindow::onReadyRead);
    connect(currentReply, &QNetworkReply::finished, this, &MainWindow::onResponseFinished);
}

void MainWindow::sendRequest()
{
    QString question = ui->inputBox->text().trimmed();
//    if (question.isEmpty()) {
//        ui->historyDisplay->append(markdownToHtml("**错误**：问题不能为空！"));
//        return;
//    }
//    ui->inputBox->clear();

    // 用户消息
    QString userMessage = QString("<div class='message user-message'>"
                                  "<img class='avatar user-avatar' src=':/icons/user.png' alt='User Avatar'>"
                                  "<div class='message-content'>%1</div>"
                                  "</div>")
                          .arg(markdownToHtml("**用户**：" + question));
    ui->historyDisplay->append(userMessage);

    // AI 消息
    QString aiMessage = QString("<div class='message ai-message'>"
                                "<div class='message-content'>%1</div>"
                                "<img class='avatar ai-avatar' src=':/icons/loading.png' alt='AI Avatar'>"
                                "</div>")
                        .arg(markdownToHtml("**AI助手**： "));

   if (question.isEmpty()) {
            ui->historyDisplay->append(markdownToHtml("**错误**：问题不能为空！"));
            return;
        }
    ui->inputBox->clear();
    ui->historyDisplay->append(aiMessage);
    currentAIAnswer.clear();
    isAIAnswerComplete = false;
    QTextCursor cursor = ui->historyDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1);
    currentAICursorPosition = cursor.position();

    QUrl url("http://127.0.0.1:8080/api/chat/");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["question"] = question;
    if (!fileContent.isEmpty()) {
        json["file_content"] = fileContent;
        fileContent.clear();
    }

    QByteArray data = QJsonDocument(json).toJson();

    if (currentReply) {
        currentReply->abort();
        currentReply->deleteLater();
    }

    currentReply = networkManager->post(request, data);
    connect(currentReply, &QNetworkReply::readyRead, this, &MainWindow::onReadyRead);
    connect(currentReply, &QNetworkReply::finished, this, &MainWindow::onResponseFinished);
}

// 修改appendMessage方法
// 修改消息附加函数
void MainWindow::appendMessage(const QString& role, const QString& content)
{
    QString html = QString(R"(
        <div class="message %1-message">
            <img class="avatar" src=":/icons/%2.png">
            <div class="message-content">%3</div>
        </div>
    )").arg(role)
     .arg(role)
     .arg(markdownToHtml(content));

    QTextCursor cursor(ui->historyDisplay->document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml(html);

    // 保留原有滚动逻辑
    QScrollBar* scrollBar = ui->historyDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void MainWindow::onReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
        if (!reply || reply != currentReply) return;

        QByteArray response = reply->readAll();
        QString buffer = QString::fromUtf8(response).replace("\r\n", "\n");

        foreach(const QString& line, buffer.split("\n", QString::SkipEmptyParts)) {
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(), &error);
            if (error.error == QJsonParseError::NoError) {
                QJsonObject json = doc.object();
                if (json.contains("answer")) {
                    QString answer = json["answer"].toString();
                    if (answer.startsWith("[思考]")) {
                        currentAIAnswer += "<span style='color:gray'>" + answer + "</span>";
                    } else {
                        currentAIAnswer += answer;
                    }
                    updateAnswerDisplay();  // 修复：改为调用 updateAnswerDisplay 而不是 on_upLoadFileBtn_clicked
                }
                else if (json.contains("error")) {
                    ui->historyDisplay->append(markdownToHtml("<span style='color:red'>**错误**：" + json["error"].toString() + "</span>"));
                }
            }
        }
}

void MainWindow::updateAnswerDisplay()
{
    QTextCursor cursor(ui->historyDisplay->document());
     cursor.setPosition(currentAICursorPosition);
     cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

     cursor.removeSelectedText();
     cursor.insertHtml(currentAIAnswer);
     ui->historyDisplay->verticalScrollBar()->setValue(ui->historyDisplay->verticalScrollBar()->maximum());
}

//上传文件
void MainWindow::on_upLoadFileBtn_clicked()
{
    QString filter = "所有支持格式 (*.txt *.pdf *.doc *.docx *.xlsx *.xls *.csv);;"
                       "文本文件 (*.txt);;PDF文件 (*.pdf);;Word文档 (*.doc *.docx);;"
                       "Excel文件 (*.xlsx *.xls);;CSV文件 (*.csv)";

    // 允许选择多个文件
    QStringList fileNames = QFileDialog::getOpenFileNames(
           this,
           tr("选择文件"),
           QDir::homePath(),
           filter
       );

       // 处理每个文件
       foreach (const QString &fileName, fileNames) {
           if (uploadedFiles.contains(fileName)) {
               appendErrorMessage(QString("文件已上传：%1").arg(fileName));
               continue;
           }

           QFile file(fileName);
           if (!file.open(QIODevice::ReadOnly)) {
               appendErrorMessage(QString("无法打开文件：%1").arg(fileName));
               continue;
           }

           QString content;
           try {
               if (fileName.endsWith(".csv", Qt::CaseInsensitive)) {
                   QTextStream in(&file);
                   in.setCodec("UTF-8");
                   QStringList lines;

                   // 读取并过滤空行
                   while (!in.atEnd()) {
                       QString line = in.readLine().trimmed();
                       if (!line.isEmpty()) lines << line;
                   }

                   if (lines.isEmpty()) {
                       ui->historyDisplay->append(markdownToHtml("<span style='color:red'>**错误**：CSV文件为空</span>"));
                       return;
                   }

                   // 自动检测分隔符
                   QChar delimiter = detectDelimiter(lines.first());
                   QStringList headers = parseCSVLine(lines.first(), delimiter);

                   // 构建Markdown表格
                   QStringList mdTable;
                   mdTable << "| " + headers.join(" | ") + " |";
                   // 修改这一行
                   mdTable << "| " + QString("---").repeated(headers.size()).split("").mid(1, headers.size() - 1).join(" | ") + " |";

                   for (int i = 1; i < lines.size(); ++i) {
                       QStringList fields = parseCSVLine(lines[i], delimiter);
                       // 自动对齐列数
                       while (fields.size() < headers.size()) fields << "";
                       if (fields.size() > headers.size()) fields = fields.mid(0, headers.size());

                       mdTable << "| " + fields.join(" | ") + " |";
                   }

                   fileContent = mdTable.join("\n");
               } else if (fileName.endsWith(".txt", Qt::CaseInsensitive)) {
                   QTextStream in(&file);
                   QTextCodec *codec = QTextCodec::codecForName("UTF-8");
                   in.setAutoDetectUnicode(true);
                   in.setCodec(codec);
                   fileContent = in.readAll();
               }
               else if (fileName.endsWith(".doc") || fileName.endsWith(".docx")) {
                   QAxObject word("Word.Application");
                   word.setProperty("Visible", false);  // 隐藏Word窗口
                   QAxObject* documents = word.querySubObject("Documents");
                   QAxObject* document = documents->querySubObject("Open(const QString&)", fileName);
                   if (document) {
                       fileContent = document->querySubObject("Range()")->dynamicCall("Text()").toString();
                       document->dynamicCall("Close(Boolean)", false);
                   }
                   word.dynamicCall("Quit()");
               }
               else if (fileName.endsWith(".xls") || fileName.endsWith(".xlsx")) {
                   QAxObject excel("Excel.Application");
                   excel.setProperty("Visible", false);
                   QAxObject* workbooks = excel.querySubObject("Workbooks");
                   QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", fileName);
                   if (workbook) {
                       QAxObject* worksheet = workbook->querySubObject("Worksheets(int)", 1);
                       QAxObject* usedRange = worksheet->querySubObject("UsedRange");
                       QAxObject* rows = usedRange->querySubObject("Rows");
                       QAxObject* columns = usedRange->querySubObject("Columns");
                       int rowCount = rows->property("Count").toInt();
                       int colCount = columns->property("Count").toInt();

                       // 优化表格格式
                       QStringList tableData;
                       for (int row = 1; row <= rowCount; ++row) {
                           QStringList rowData;
                           for (int col = 1; col <= colCount; ++col) {
                               QAxObject* cell = worksheet->querySubObject("Cells(int,int)", row, col);
                               QVariant cellValue = cell->property("Value");
                               rowData << formatCellValue(cellValue);
                               delete cell;
                           }
                           tableData << rowData.join(" | ");  // 使用 | 作为列分隔符
                       }
                       fileContent = tableData.join("\n");  // 使用换行符分隔行
                       workbook->dynamicCall("Close(Boolean)", false);
                   }
                   excel.dynamicCall("Quit()");
               }
               else if (fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
                   // 使用系统命令读取PDF内容（需要安装pdftotext）
                   QString pdfText;
                   QProcess process;
                   process.start("pdftotext", QStringList() << "-layout" << fileName << "-");
                   if (process.waitForFinished()) {
                       pdfText = process.readAllStandardOutput();
                   }
                   if (pdfText.isEmpty()) {
                       ui->historyDisplay->append(markdownToHtml("<span style='color:red'>**错误**：无法读取PDF文件，请确保已安装pdftotext工具</span>"));
                       return;
                   }
                   fileContent = pdfText;
               }

               uploadedFiles.append(fileName);
               fileContents[fileName] = content;
               appendSuccessMessage(QString("成功上传：%1").arg(fileName));
           } catch (const std::exception& e) {
               appendErrorMessage(QString("处理失败[%1]：%2").arg(fileName).arg(e.what()));
           }
           file.close();

           if (fileContent.isEmpty()) {
               ui->historyDisplay->append(markdownToHtml("<span style='color:red'>**错误**：文件内容为空或读取失败</span>"));
               return;
           }

       //    ui->historyDisplay->append(markdownToHtml("**文件内容已解析**：\n" + fileContent));

           if (fileContent.isEmpty()) {
               ui->historyDisplay->append(markdownToHtml("<span style='color:red'>**错误**：文件内容为空或读取失败</span>"));
               return;
           }

           // 显示处理后的内容
           qDebug() << "Processed content:" << fileContent;
           ui->historyDisplay->append(markdownToHtml("**文件内容已读取**"));
       }

}

// CSV解析函数（支持引号包裹字段）
QStringList MainWindow::parseCSVLine(const QString &line, QChar delimiter)
{
    QStringList fields;
    QString buffer;
    bool inQuotes = false;
    QChar prevChar;

    foreach (QChar c, line) {
        if (c == '"') {
            if (inQuotes && prevChar == '"') { // 处理双引号转义
                buffer += '"';
                prevChar = QChar();
                continue;
            }
            inQuotes = !inQuotes;
        } else if (c == delimiter && !inQuotes) {
            fields << buffer.trimmed();
            buffer.clear();
        } else {
            buffer += c;
        }
        prevChar = c;
    }
    fields << buffer.trimmed(); // 添加最后一个字段

    // 清理字段中的残留引号
    fields.replaceInStrings(QRegularExpression("^\"|\"$"), "");
    return fields;
}


// 新增方法：格式化单元格值
QString MainWindow::formatCellValue(const QVariant& value)
{
    if (value.isNull()) {
        return "";  // 空单元格
    }
    else if (value.type() == QVariant::String) {
        return value.toString();
    }
    else if (value.type() == QVariant::Double) {
        return QString::number(value.toDouble(), 'f', 2);
    }
    else if (value.type() == QVariant::Date) {
        return value.toDate().toString("yyyy-MM-dd");
    }
    else {
        return value.toString();
    }
}

void MainWindow::onResponseFinished()
{
    if (currentReply->error() != QNetworkReply::NoError &&
        currentReply->error() != QNetworkReply::OperationCanceledError) {
        ui->historyDisplay->append(markdownToHtml("<span style='color:red'>**网络错误**：" + currentReply->errorString() + "</span>"));
    }
    currentReply->deleteLater();
    currentReply = nullptr;
}

//暂停思考
void MainWindow::on_StopBtn_clicked()
{
    if (currentReply) {
        currentReply->abort();
        ui->historyDisplay->append(markdownToHtml("<span style='color:blue'>**操作**：已停止思考</span>"));
    }
}

QString MainWindow::markdownToHtml(const QString& markdown)
{
    if (markdown.isEmpty()) {
                return "<span style='color:red'>**错误**：AI 返回内容为空</span>";
            }

        QString html = markdown;

        // 标题处理
        html.replace(QRegularExpression("^#{6} (.*)$", QRegularExpression::MultilineOption), "<h6>\\1</h6>");
        html.replace(QRegularExpression("^#{5} (.*)$", QRegularExpression::MultilineOption), "<h5>\\1</h5>");
        html.replace(QRegularExpression("^#{4} (.*)$", QRegularExpression::MultilineOption), "<h4>\\1</h4>");
        html.replace(QRegularExpression("^### (.*)$", QRegularExpression::MultilineOption), "<h3>\\1</h3>");
        html.replace(QRegularExpression("^## (.*)$", QRegularExpression::MultilineOption), "<h2>\\1</h2>");
        html.replace(QRegularExpression("^# (.*)$", QRegularExpression::MultilineOption), "<h1>\\1</h1>");

        // 列表处理
        html.replace(QRegularExpression("^\\* (.*)$", QRegularExpression::MultilineOption), "<li>\\1</li>");
        html.replace(QRegularExpression("^\\d+\\. (.*)$", QRegularExpression::MultilineOption), "<li>\\1</li>");
        html = "<ul>" + html + "</ul>";

        // 代码块处理
        html.replace(QRegularExpression("```(.*?)```", QRegularExpression::DotMatchesEverythingOption),
                    "<pre style='background:#f5f5f5;padding:10px;border-radius:5px'>\\1</pre>");

        // 强调处理
        html.replace(QRegularExpression("\\*\\*(.*?)\\*\\*"), "<strong>\\1</strong>");
        html.replace(QRegularExpression("\\*(.*?)\\*"), "<em>\\1</em>");

        // 链接处理
        html.replace(QRegularExpression("\\[(.*?)\\]\\((.*?)\\)"),
                    "<a href='\\2' style='color:#0066cc;text-decoration:none'>\\1</a>");

        // 表格处理
        html.replace(QRegularExpression("\\|(.+?)\\|", QRegularExpression::MultilineOption),
                    "<table style='border-collapse:collapse;width:100%'>\\1</table>");
        html.replace(QRegularExpression("\\|\\-+\\|"),
                    "<tr style='border-bottom:1px solid #ddd'>");
        html.replace(QRegularExpression("\\|(.*?)\\|"),
                    "<td style='padding:8px;border-bottom:1px solid #ddd'>\\1</td>");

        // 换行处理
        html.replace("\n", "<br>");

        return html;
}
void MainWindow::playSpeech()
{
    // 获取当前光标位置
    QTextCursor cursor = ui->historyDisplay->textCursor();
    int currentPosition = cursor.position();

    if (!currentAIAnswer.isEmpty()) {
        // 移除 HTML 标签
        QString plainText = currentAIAnswer;
        QTextDocument doc;
        doc.setHtml(plainText);
        plainText = doc.toPlainText();

        // 如果正在播放，先停止
        if (speech->state() == QTextToSpeech::Speaking) {
            speech->stop();
        }
        speech->say(plainText);
    }

    // 恢复光标位置
    cursor.setPosition(currentPosition);
    ui->historyDisplay->setTextCursor(cursor);
}

void MainWindow::onAnchorClicked(const QUrl &link)
{
    if (link.scheme() == "play" && link.host() == "speech") {
        if (!isAIAnswerComplete) {
            QMessageBox::information(this, "提示", "回答尚未完成，无法点击");
            return;
        }

        if (speech->state() == QTextToSpeech::Speaking) {
            speech->stop();
        } else {
            playSpeech();
        }
    }
}


//窗口界面优化
void MainWindow::Beawidget()
{
    //窗体去边框
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    this->setWindowTitle("云曦智划");

    //窗体圆角化
    QBitmap bmp(this->size());
    bmp.fill();

    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawRoundedRect(bmp.rect(),20,20);

    setMask(bmp);

}

//窗口可移动
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    QPoint y =event->globalPos(); //鼠标相对于桌面左上角的位置，鼠标全局位置
    QPoint x =y-this->z;
    this->move(x);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    QPoint y =event->globalPos(); //鼠标相对于桌面左上角，鼠标全局位置
    QPoint x =this->geometry().topLeft();   //窗口左上角相对于桌面位置，窗口位置
    this-> z =y-x ;//定值不变
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    this->z=QPoint();
}


//鼠标双击特效
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    //判断是否为鼠标左键双击
    if(event->button() == Qt::LeftButton)
    {
        QLabel * label = new QLabel(this);
        QMovie * movie = new QMovie("://icons/mouse.gif");//加载gif图片
        //设置label自动适应gif的大小
        label->setScaledContents(true);

        label->setMovie(movie);
        //这里为了调用move方便，进行resize，需要知道的是gif的大小本来也就是150*150
        label->resize(180,180);
        label->setStyleSheet("background-color:rgba(0,0,0,0);");
        //设置鼠标穿透
        label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        //让label的中心在当前鼠标双击位置
        label->move(event->pos().x()-label->width()/2,event->pos().y()-label->height()/2);
        //开始播放gif
        movie->start();

        label->show();

        //绑定QMovie的信号，判断gif播放次数
        connect(movie, &QMovie::frameChanged, [=](int frameNumber) {
            if (frameNumber == movie->frameCount() - 1)//gif播放次数为1，关闭标签
                label->close();
        });
    }
}

//按钮的美化
void MainWindow::BeaBtn()
{
    //退出按钮
    connect(ui->exitBtn, &QPushButton::clicked,this, &MainWindow::close);

    ui->exitBtn->setStyleSheet(
                         //正常状态样式
                         "QPushButton{"
                         "background-color:#ffffff;"//设置按钮背景色
                         "border-radius:40px;"//设置圆角半径
                         "}"
                         "QPushButton:hover{"
                                              "background-color:#8DB6E7;"//设置按钮点击时的背景颜色
                                              "}");
    ui->sendButton->setStyleSheet(
                         //正常状态样式
                         "QPushButton{"
                         "background-color:#ffffff;"//设置按钮背景色
                         "border-radius:50px;"//设置圆角半径
                         "}"
                         "QPushButton:hover{"
                                              "background-color:#8DB6E7;"//设置按钮点击时的背景颜色
                                              "}");

    ui->StopBtn->setStyleSheet(
                         //正常状态样式
                         "QPushButton{"
                         "background-color:#ffffff;"//设置按钮背景色
                         "border-radius:50px;"//设置圆角半径
                         "}"
                         "QPushButton:hover{"
                                              "background-color:#8DB6E7;"//设置按钮点击时的背景颜色
                                              "}");
    ui->upLoadFileBtn->setStyleSheet(
                         //正常状态样式
                         "QPushButton{"
                         "background-color:#ffffff;"//设置按钮背景色
                         "border-radius:40px;"//设置圆角半径
                         "}"
                         "QPushButton:hover{"
                                              "background-color:#8DB6E7;"//设置按钮点击时的背景颜色
                                              "}");
    ui->VoiceBtn->setStyleSheet(
                         //正常状态样式
                         "QPushButton{"
                         "background-color:#ffffff;"//设置按钮背景色
                         "border-radius:40px;"//设置圆角半径
                         "}"
                         "QPushButton:hover{"
                                              "background-color:#8DB6E7;"//设置按钮点击时的背景颜色
                                              "}");

    ui->InterSearchBtn->setStyleSheet(
                         //正常状态样式
                         "QPushButton{"
                         "background-color:#ffffff;"//设置按钮背景色
                         "border-radius:40px;"//设置圆角半径
                         "}"
                         "QPushButton:hover{"
                                              "background-color:#8DB6E7;"//设置按钮点击时的背景颜色
                                              "}");
    ui->SaveBtn->setStyleSheet(
                         //正常状态样式
                         "QPushButton{"
                         "background-color:#ffffff;"//设置按钮背景色
                         "border-radius:40px;"//设置圆角半径
                         "}"
                         "QPushButton:hover{"
                                              "background-color:#8DB6E7;"//设置按钮点击时的背景颜色
                                              "}");

    ui->inputBox->setStyleSheet(
                         //正常状态样式
                         "QLineEdit{"
                         "background-color:rgb(183, 199, 221);"//设置背景色
                         "border-radius:40px;"//设置圆角半径
                         "}");
    //滚动条
    ui->historyDisplay->verticalScrollBar()->setStyleSheet(
        "QScrollBar:vertical {"
        "    border: none;"
        "    background: #f8f9fa;"
        "    width: 8px;"
        "    margin: 0px 0px 0px 0px;"
        "}"
    );

    ui->historyDisplay->document()->setDefaultStyleSheet(R"(
        /* 统一左对齐修改 */
        .message {
            margin: 8px 0;
            clear: both;
            float: left; /* 新增 */
        }

        /* 用户消息保持原有样式仅修改对齐 */
        .user-message {
            background-color: #e3f2fd;
            margin-left: 8px; /* 调整边距 */
            margin-right: auto;
        }

        /* AI消息保持原有样式仅修改对齐 */
        .ai-message {
            background-color: #f5f5f5;
            margin-left: 8px; /* 调整边距 */
            margin-right: auto;
        }

        /* 保留原有其他样式 */
        .avatar {
            width: 24px;
            height: 24px;
            border-radius: 50%;
            vertical-align: middle;
            margin-right: 8px;
        }

        .message-content {
            padding: 12px;
            border-radius: 8px;
            max-width: 80%;
        }
    )");
}

//联网搜索
void MainWindow::on_InterSearchBtn_clicked()
{

}
// 新增的私有函数实现
// 修改获取token的postData拼接方式
void MainWindow::getBaiduToken() {
    QUrl tokenUrl("https://aip.baidubce.com/oauth/2.0/token");
    QNetworkRequest request(tokenUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QString postData = QString("grant_type=client_credentials&"
                             "client_id=J26ox050nZKiB7SiMOCZjbok&"  // 添加&分隔符
                             "client_secret=Z7kMWchU9CIbobrSfbhve8Bd6xJV3urq");
}
void MainWindow::on_VoiceBtn_clicked() {
    if (audioRecorder->state() == QAudioRecorder::StoppedState) {
        // 设置正确的音频格式
        QAudioFormat format;
        format.setSampleRate(16000);
        format.setChannelCount(1);
        format.setSampleSize(16);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::SignedInt);

        // 检查格式是否支持
        QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
        if (!info.isFormatSupported(format)) {
            QMessageBox::warning(this, "错误", "设备不支持所需的音频格式");
            return;
        }

        outputFile.setFileName(QDir::tempPath() + "/recording_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".pcm");
        if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QMessageBox::warning(this, "错误", "无法创建临时文件");
            durationTimer->start(1000); // 每秒更新一次显示
            return;
        }else{
            durationTimer->stop();
        }
        outputFile.close();

        audioRecorder->setAudioInput(info.deviceName());
        audioRecorder->setOutputLocation(QUrl::fromLocalFile(outputFile.fileName()));
        audioRecorder->record();
//        ui->VoiceBtn->setText("停止录音");
    } else {
        audioRecorder->stop();
//        ui->VoiceBtn->setText("开始录音");
        // 增加延迟时间，确保文件写入完成
        QTimer::singleShot(1000, this, [this]() {
            // 获取实际录音时长
            qint64 durationMs = recordTimer.elapsed();
            if (durationMs < 1000) {
                QMessageBox::warning(this, "错误", QString("录音时间太短（%1ms），请至少录制1秒").arg(durationMs));
                return;
            }
            processAudioFile();
        });
    }
}
void MainWindow::processAudioFile() {
    QFile audioFile(outputFile.fileName());
    if (!audioFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法读取录音文件");
        return;
    }

    QByteArray audioData = audioFile.readAll();
    audioFile.close();

    // 验证音频长度
    int byteCount = audioData.size();
    int durationMs = byteCount / (16000 * 2); // 计算时长（16kHz, 16bit=2字节）
    if (durationMs < 1000) {
        QMessageBox::warning(this, "错误", "录音时间太短，请至少录制1秒");
        return;
    }

    // 发送到百度API
    QUrl asrUrl("https://vop.baidubce.com/pro_api");
    QUrlQuery query;
    query.addQueryItem("dev_pid", "80001");
    query.addQueryItem("cuid", deviceID);
    query.addQueryItem("token", baiduAccessToken);
    asrUrl.setQuery(query);

    QNetworkRequest request(asrUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "audio/pcm;rate=16000");

    QNetworkReply* reply = networkManager->post(request, audioData);
    connect(reply, &QNetworkReply::finished, [this, reply]() { handleSpeechRecognitionReply(reply); });
}
void MainWindow::handleSpeechRecognitionReply(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
           QByteArray response = reply->readAll();
           qDebug() << "原始响应:" << response;  // 添加调试输出

           QJsonParseError parseError;
           QJsonDocument doc = QJsonDocument::fromJson(response, &parseError);

           if (parseError.error == QJsonParseError::NoError) {
               QJsonObject json = doc.object();
               if (json.contains("result")) {
                   QString text = json["result"].toArray()[0].toString();
                   ui->inputBox->setText(text);
               } else if (json.contains("err_msg")) {
                   ui->historyDisplay->append("识别错误: " + json["err_msg"].toString());
               }
           } else {
               ui->historyDisplay->append("响应解析错误: " + parseError.errorString());
           }
       } else {
           ui->historyDisplay->append("网络错误: " + reply->errorString());
       }
       reply->deleteLater();
}


QString MainWindow::readExcelFile(const QString& filePath)
{
    QScopedPointer<QAxObject> excel;
    QScopedPointer<QAxObject> workbooks;
    QScopedPointer<QAxObject> workbook;

    try {
        if (!isExcelAvailable()) {
            throw std::runtime_error("未检测到Excel安装");
        }

        excel.reset(new QAxObject("Excel.Application", this));
        excel->setProperty("Visible", false);
        excel->setProperty("DisplayAlerts", false);

        workbooks.reset(excel->querySubObject("Workbooks"));
        workbook.reset(workbooks->querySubObject("Open(const QString&)", filePath));

        QAxObject* worksheets = workbook->querySubObject("Worksheets");
        QScopedPointer<QAxObject> worksheet(worksheets->querySubObject("Item(int)", 1));
        QScopedPointer<QAxObject> usedRange(worksheet->querySubObject("UsedRange"));

        QScopedPointer<QAxObject> rows(usedRange->querySubObject("Rows"));
        QScopedPointer<QAxObject> columns(usedRange->querySubObject("Columns"));
        int rowCount = rows->property("Count").toInt();
        int colCount = columns->property("Count").toInt();

        // 构建JSON数据
        QJsonArray jsonData;
        for (int row = 1; row <= rowCount; ++row) {
            QJsonObject rowData;
            for (int col = 1; col <= colCount; ++col) {
                QScopedPointer<QAxObject> cell(worksheet->querySubObject("Cells(int,int)", row, col));
                QVariant value = cell->property("Value");
                QString cellValue = formatCellValue(value);
                rowData[QString("col%1").arg(col)] = cellValue.trimmed();
            }
            jsonData.append(rowData);
        }

        workbook->dynamicCall("Close(Boolean)", false);
        excel->dynamicCall("Quit()");

        // 将JSON数据转换为字符串
        QJsonDocument doc(jsonData);
        return doc.toJson(QJsonDocument::Compact);

    } catch (const std::exception& e) {
        if (workbook) workbook->dynamicCall("Close(Boolean)", false);
        if (excel) excel->dynamicCall("Quit()");
        throw;
    }
}

QString MainWindow::readCSVFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("无法打开CSV文件");
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    QStringList lines;

    // 读取并过滤空行
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) lines << line;
    }

    // 自动检测分隔符
    QChar delimiter = detectDelimiter(lines.first());
    QStringList headers = parseCSVLine(lines.first(), delimiter);

    // 构建Markdown表格
    QStringList mdTable;
    mdTable << "| " + headers.join(" | ") + " |";
    // 修改这一行
    mdTable << "| " + QString("---").repeated(headers.size()).split("").mid(1, headers.size() - 1).join(" | ") + " |";

    for (int i = 1; i < lines.size(); ++i) {
        QStringList fields = parseCSVLine(lines[i], delimiter);
        // 自动对齐列数
        while (fields.size() < headers.size()) fields << "";
        if (fields.size() > headers.size()) fields = fields.mid(0, headers.size());

        mdTable << "| " + fields.join(" | ") + " |";
    }

    fileContent = mdTable.join("\n");

    // 构建JSON数据
    QJsonArray jsonData;
    for (int i = 1; i < lines.size(); ++i) {
        QStringList rowData = lines[i].split(delimiter);
        rowData.replaceInStrings("\"", "");
        QJsonObject rowObject;
        for (int j = 0; j < headers.size(); ++j) {
            rowObject[headers[j].trimmed()] = rowData[j].trimmed();
        }
        jsonData.append(rowObject);
    }

    // 将JSON数据转换为字符串
    QJsonDocument doc(jsonData);
    return doc.toJson(QJsonDocument::Compact);
}


bool MainWindow::isExcelAvailable()
{
    QAxObject* excel = new QAxObject("Excel.Application", this);
    if (!excel || excel->isNull()) {
        delete excel;
        return false;
    }
    delete excel;
    return true;
}

QChar MainWindow::detectDelimiter(const QString& sampleLine)
{
    QMap<QChar, int> delimiterCounts;

    // 候选分隔符
    QList<QChar> candidates = {',', ';', '\t', '|'};

    foreach(QChar c, candidates) {
        delimiterCounts[c] = sampleLine.count(c);
    }

    // 选择出现次数最多的分隔符
    auto maxIt = std::max_element(delimiterCounts.begin(), delimiterCounts.end());
    return (maxIt != delimiterCounts.end() && *maxIt > 0) ?
           delimiterCounts.key(*maxIt) :
           ',';
}
void MainWindow::appendErrorMessage(const QString& message)
{
    ui->ProcessBrowser->append(
        markdownToHtml(QString("<span style='color:red'>**错误**：%1</span>").arg(message))
    );
}

void MainWindow::appendSuccessMessage(const QString& message)
{
    ui->ProcessBrowser->append(
        markdownToHtml(QString("<span style='color:green'>**成功**：%1</span>").arg(message))
    );
}

void MainWindow::appendMarkdownContent(const QString& content)
{
    ui->ProcessBrowser->append(
        markdownToHtml(QString("**文件内容**：\n```\n%1\n```").arg(content))
    );
}

void MainWindow::sendJsonToBackend(const QString& jsonData)
{
    QUrl url("http://127.0.0.1:8080/api/chat/"); // 后端API地址
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["data"] = QJsonDocument::fromJson(jsonData.toUtf8()).array();

    QByteArray postData = QJsonDocument(json).toJson();

    QNetworkReply* reply = networkManager->post(request, postData);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleBackendResponse(reply);
    });
}

void MainWindow::handleBackendResponse(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        if (doc.isObject()) {
            QJsonObject json = doc.object();
            if (json.contains("answer")) {
                QString answer = json["answer"].toString();
                // 将Markdown格式的回答转为HTML
                QString htmlAnswer = markdownToHtml(answer);
                ui->ProcessBrowser->setHtml(htmlAnswer);
            } else {
                appendErrorMessage("后端返回格式错误");
            }
        } else {
            appendErrorMessage("后端返回数据解析失败");
        }
    } else {
        appendErrorMessage(QString("网络错误：%1").arg(reply->errorString()));
    }
    reply->deleteLater();
}
void MainWindow::sendCombinedRequest(const QString& question, const QString& fileData)
{
    QUrl url("http://127.0.0.1:8080/api/chat"); // 后端API地址
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 设置请求超时（10秒）
//    request.setTransferTimeout(10000);

    // 构建组合请求体
    QJsonObject requestBody;
    requestBody["question"] = question;
    requestBody["file_data"] = QJsonDocument::fromJson(fileData.toUtf8()).array();

    QByteArray postData = QJsonDocument(requestBody).toJson();

    // 显示等待提示
    ui->historyDisplay->append(markdownToHtml("**AI正在分析中...**"));

    // 发送请求
    QNetworkReply* reply = networkManager->post(request, postData);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleCombinedResponse(reply);
    });

    // 处理网络错误
//    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply](QNetworkReply::NetworkError error) {
//        if (error != QNetworkReply::NoError) {
//            appendErrorMessage(QString("网络错误：%1").arg(reply->errorString()));
//            reply->deleteLater();
//        }
//    });
}
void MainWindow::handleCombinedResponse(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        appendErrorMessage(QString("请求失败：%1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    reply->deleteLater();

    // 检查响应是否为空
    if (response.isEmpty()) {
        appendErrorMessage("后端返回数据为空");
        return;
    }

    // 解析 JSON 数据
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(response, &parseError);

    // 检查 JSON 解析是否成功
    if (parseError.error != QJsonParseError::NoError) {
        appendErrorMessage(QString("JSON 解析失败：%1").arg(parseError.errorString()));
        return;
    }

    // 检查 JSON 是否为对象
    if (!doc.isObject()) {
        appendErrorMessage("返回数据格式异常：不是有效的 JSON 对象");
        return;
    }

    QJsonObject json = doc.object();

    // 检查是否包含 "answer" 字段
    if (!json.contains("answer")) {
        appendErrorMessage("返回数据格式异常：缺少 'answer' 字段");
        return;
    }

    // 检查 "answer" 字段是否为字符串
    if (!json["answer"].isString()) {
        appendErrorMessage("返回数据格式异常：'answer' 字段不是字符串");
        return;
    }

    // 获取并显示 AI 的回答
    QString answer = json["answer"].toString();
    QString html = markdownToHtml(answer);
    ui->historyDisplay->setHtml(html);
}


