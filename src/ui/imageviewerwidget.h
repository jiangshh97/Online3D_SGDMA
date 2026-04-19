#ifndef IMAGEVIEWERWIDGET_H
#define IMAGEVIEWERWIDGET_H

#include <QCoreApplication>
#include <QThread>
#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <opencv2/opencv.hpp>

class ImageViewerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageViewerWidget(QWidget *parent = nullptr);

    void imageShow(const cv::Mat &apicture);

private:
    void qimageShow(QImage apicture);

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QWidget::paintEvent(event);
        if (!m_pixmap.isNull())
        {
            QPainter painter(this);
            QPixmap drawPixmap = m_pixmap;
            if (m_pixmap.width() > width() || m_pixmap.height() > height())
            {
                drawPixmap = m_pixmap.scaled(
                    size(),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation);
            }
            int x = (width() - drawPixmap.width()) / 2;
            int y = (height() - drawPixmap.height()) / 2;
            painter.drawPixmap(x, y, drawPixmap);
        }
    }

signals:
    void sendImageViewerMessage(QString module, QString msg, int code);

private:
    QPixmap m_pixmap;
};

#endif // IMAGEVIEWERWIDGET_H
