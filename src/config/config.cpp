#include "config.h"

Config::Config()
{
    QDir appDir = QCoreApplication::applicationDirPath();
    appDir.cdUp();
    cfgFilePath = appDir.path() + cfgFilePath;
    QDir mDir(cfgFilePath);
    if (!mDir.exists())
        mDir.mkpath(cfgFilePath); // Create the configuration directory if it does not exist
}

void Config::configRead()
{
    try
    {
        std::ifstream cfgParams((cfgFilePath + cfgFileName).toStdString());
        {
            cereal::JSONInputArchive archive(cfgParams);
            archive(cereal::make_nvp("Reconstruction Config", reconstructParams)); // Deserialize the reconstruction parameters
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl; // Log the caught exception
        return;
    }
    catch (...)
    {
        // Catch any unknown exceptions
        std::cerr << "Unknown exception caught!" << std::endl;
        return;
    }
}

void Config::configWrite()
{
    try
    {
        std::ofstream cfgParams((cfgFilePath + cfgFileName).toStdString());
        {
            cereal::JSONOutputArchive archive(cfgParams);
            archive(cereal::make_nvp("Reconstruction Config", reconstructParams)); // Serialize the reconstruction parameters
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl; // Log the caught exception
    }
    catch (...)
    {
        // Catch any unknown exceptions
        std::cerr << "Unknown exception caught!" << std::endl;
        return;
    }
}