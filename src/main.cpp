#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include "QVTKOpenGLNativeWidget.h"
#include <QMetaType>
Q_DECLARE_METATYPE(cv::Mat)

int main(int argc, char *argv[])
{
    // High DPI display
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    // VTK initialization
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    qRegisterMetaType<cv::Mat>("cv::Mat");

    omp_set_num_threads(omp_get_max_threads());

    QApplication app(argc, argv);

    Config::get_instance()->configRead();

    MainWindow w;
    w.show();

    return app.exec();
}