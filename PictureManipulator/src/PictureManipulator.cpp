/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 11:40:56 +0800
 * @LastEditTime: 2019-10-14 15:30:01 +0800
 * @LastEditors: 
 * @Description: 
 */

#include "PictureManipulator.h"

PictureManipulator::PictureManipulator() : stm32()
{
    filename = "";
    fps = 0;
    width_video_size = 0;
    height_video_size = 0;
}

PictureManipulator::PictureManipulator(string serial_port_device,
                                       string path,
                                       double fps,
                                       int width_video_size,
                                       int height_video_size)
    : filename(path), fps(fps),
      width_video_size(width_video_size),
      height_video_size(height_video_size),
      video_size(width_video_size, height_video_size),
      stm32(serial_port_device)
{
    if (*filename.end() == '/')
    {
        filename += std::to_string(time(NULL));
    }
    else
    {
        filename += '/' + std::to_string(time(NULL));
    }
    src_video.open(filename + "SRC.avi", CV_FOURCC('M', 'J', 'P', 'G'), fps, video_size);
    fin_video.open(filename + "FIN.avi", CV_FOURCC('M', 'J', 'P', 'G'), fps, video_size);

}

PictureManipulator::~PictureManipulator()
{
}

/**
 * @Author: 王占坤
 * @Description: 用于图形化显示调整参数
 * @Param: 
 * @Return: 
 * @Throw: 
 */
void PictureManipulator::adjustParameter()
{
    string window_name = "Parameter";
    namedWindow(window_name);
    createTrackbar("Armordetector.gray_thresh", window_name, &armor_detector.gray_thresh, 255);
    createTrackbar("ArmorDetector.single_color_img", window_name, &armor_detector.single_color_thresh, 255);
}