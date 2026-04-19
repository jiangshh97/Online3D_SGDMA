#ifndef YOLOSEGINFERENCE_H
#define YOLOSEGINFERENCE_H

#include <QObject>
#include "yolo/yolo_seg.h"
#include "common/workpiece_params.h"

class YoLoSegInference : public QObject
{
    Q_OBJECT
public:
    explicit YoLoSegInference(const std::string &eigenPath, QObject *parent = nullptr);

    /**
     * @brief YOLO segmentation inference
     * @param src       Input image
     * @param res       Output image with detection results
     * @param objs      Output detection results
     * @param params    Configuration parameters
     */
    void runInference(const cv::Mat &src, cv::Mat &res, std::vector<yolo::Object> &new_objs, const reconstruction::YOLOParams &params);

private:
    void yoloinfer(const cv::Mat &src, std::vector<yolo::Object> &objs, const reconstruction::YOLOParams &params); // YOLO inference

private:
    std::unique_ptr<yolo::seg::YOLO> yolo = nullptr;

    cv::Size size = cv::Size(1024, 1024);

    int seg_h = 256;
    int seg_w = 256;

    int seg_channels = 32;

    const std::vector<std::string> CLASS_NAMES = {
        "hook",
        "type2",
        "type3",
        "type4"};

    const std::vector<std::vector<unsigned int>> COLORS = {
        {0, 128, 255},
        {128, 255, 0},
        {255, 0, 128},
        {0, 255, 128}};

    const std::vector<std::vector<unsigned int>> MASK_COLORS = {
        {0, 128, 255},
        {128, 255, 0},
        {255, 0, 128},
        {0, 255, 128}};
};

#endif // YOLOSEGINFERENCE_H

