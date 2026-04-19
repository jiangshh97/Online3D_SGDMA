#ifndef LOGWRITE_H
#define LOGWRITE_H

#include <QObject>
#include <QDateTime>
#include <QDebug>
#include <QStatusBar>
#include <QTextEdit>
#include <QDir>
#include <QApplication>
#include <iostream>
#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>

class LogWrite : public QObject
{
    Q_OBJECT
public:
    // Singleton pattern
    static LogWrite *get_instance()
    {
        static LogWrite log;
        return &log;
    }

    void setWidgets(QStatusBar *bar, QTextEdit *operation);

    void printLog(const QString &module, const QString &info, ...);          // Display log (module information, UI-related, not thread-safe)
    void printLog(const QString &module, const QString &err, int code, ...); // Display log (module error information, UI-related, not thread-safe)

private:
    LogWrite();

    QStatusBar *statusBar = nullptr;   // Pointer to the status bar
    QTextEdit *operationLog = nullptr; // Pointer to the log widget
};

#endif // LOGWRITE_H