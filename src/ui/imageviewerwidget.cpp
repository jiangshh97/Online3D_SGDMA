#include "imageviewerwidget.h"

ImageViewerWidget::ImageViewerWidget(QWidget *parent) : QWidget(parent)
{
}

void ImageViewerWidget::imageShow(const cv::Mat &apicture)
{
    if (!apicture.empty())
    {
        cv::Mat temp = apicture.clone();
        QImage image;
        if (temp.type() == CV_8UC1)
        {
            // Convert grayscale image
            image = QImage(temp.data, temp.cols, temp.rows, temp.step, QImage::Format_Grayscale8).copy();
        }
        else if (temp.type() == CV_8UC3)
        {
            // Convert BGR image to RGB
            cv::cvtColor(temp, temp, cv::COLOR_BGR2RGB);
            image = QImage(temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888).copy();
        }
        else if (temp.type() == CV_8UC4)
        {
            // Convert RGBA image
            image = QImage(temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGBA8888).copy();
        }
        else
        {
            // Unsupported image format
            image = QImage();
        }

        qimageShow(image);
    }
    else
    {
        // Emit message if the image content is empty
        QString msg = u8"Image content is empty!";
        emit sendImageViewerMessage("Image Viewer", msg, 0);
    }
}

void ImageViewerWidget::qimageShow(QImage apicture)
{
    if (!apicture.isNull())
    {
        // Convert QImage to QPixmap and update the widget
        m_pixmap = QPixmap::fromImage(apicture);
        update();
    }
    else
    {
        // Emit message if the image content is empty
        QString msg = u8"Image content is empty!";
        emit sendImageViewerMessage("Image Viewer", msg, 0);
    }
}