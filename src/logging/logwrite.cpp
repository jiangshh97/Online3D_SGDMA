#include "logwrite.h"

static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;

LogWrite::LogWrite()
{
    plog::init(plog::debug, &consoleAppender); // Initialize the logger
}

void LogWrite::setWidgets(QStatusBar *bar, QTextEdit *operation)
{
    statusBar = bar;
    operationLog = operation;
    statusBar->setStyleSheet("color:white"); // Set the status bar text color to white
}

void LogWrite::printLog(const QString &module, const QString &info, ...)
{
    // Output to the log widget
    QString logTime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
    QString s = logTime + " " + module + " " + info;
    operationLog->setTextColor(Qt::white); // Display white text in the log widget
    operationLog->append(s);
    // Output to the status bar
    statusBar->setStyleSheet("color:white"); // Set the status bar text color to white
    statusBar->showMessage(s, 60 * 1000); // Display the log message in the status bar for 60 seconds
}

void LogWrite::printLog(const QString &module, const QString &err, int code, ...)
{
    // Output to the log widget
    QString logTime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
    QString s = logTime + " " + module + " " + err + " code: " + QString::number(code);
    operationLog->setTextColor(Qt::red); // Display red text in the log widget
    operationLog->append(s);
    // Output to the status bar
    statusBar->setStyleSheet("color:red"); // Set the status bar text color to red
    statusBar->showMessage(s); // Display the log message in the status bar
}