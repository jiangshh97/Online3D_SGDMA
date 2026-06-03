#include "yoloseginference.h"

YoLoSegInference::YoLoSegInference(const std::string &eigenPath, QObject *parent)
    : QObject(parent)
{
    if (eigenPath.empty())
    {
        throw std::invalid_argument("YOLO engine path is empty");
    }

    cudaSetDevice(0);
    yolo = std::make_unique<yolo::seg::YOLO>(eigenPath);
    yolo->make_pipe(true);
}

void YoLoSegInference::runInference(const cv::Mat &src, cv::Mat &res, std::vector<yolo::Object> &new_objs, const reconstruction::YOLOParams &params)
{
    // Convert the input image to three channels
    cv::Mat image;
    if (src.channels() == 1)
    {
        cv::cvtColor(src, image, cv::COLOR_GRAY2BGR);
    }
    else
        image = src.clone();

    // YOLO inference
    std::vector<yolo::Object> objs;
    this->yoloinfer(image, objs, params);

    // Process the results (adjust according to the trained model)
    objs.erase(
        std::remove_if(objs.begin(), objs.end(),
                       [](const yolo::Object &obj)
                       {
                           return obj.label != 0;
                       }),
        objs.end());

    // Draw the results
    yolo->draw_objects(image, res, objs, CLASS_NAMES, COLORS, MASK_COLORS);

    // Output the results
    new_objs = std::move(objs);
}

void YoLoSegInference::yoloinfer(const cv::Mat &src, std::vector<yolo::Object> &objs, const reconstruction::YOLOParams &params)
{
    yolo->copy_from_Mat(src, size);
    yolo->infer();
    yolo->postprocess(objs, params.scoreThres, params.iouThres, params.topk, seg_channels, seg_h, seg_w);
}

