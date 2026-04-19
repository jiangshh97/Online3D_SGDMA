#pragma once

#include <QCoreApplication>
#include <QDir>
#include <QFile>

// Include configuration parameters for each module
#include "reconstructioncommon.h"

// Serialization headers
#include <iostream>
#include <fstream>
#include <string>
#include "cereal/archives/json.hpp"

class Config : public QObject
{
    Q_OBJECT
public:
    // Singleton pattern
    static Config *get_instance()
    {
        static Config cfg;
        return &cfg;
    }

    void configRead();

    void configWrite();

    reconstructionParams reconstructParams; // Parameters for the model reconstruction module

private:
    Config();

    QString cfgFilePath = "/resource/config/";
    QString cfgFileName = "config.json";
};