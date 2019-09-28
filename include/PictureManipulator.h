/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 11:40:32 +0800
 * @LastEditTime: 2019-09-28 12:11:50 +0800
 * @LastEditors: 
 * @Description: 
 */
#ifndef __PICTURE_MANIPULATOR_H__
#define __PICTURE_MANIPULATOR_H__
#include "header.h"
#include "ArmorDetector.hpp"
class PictureManipulator
{
private:
    double fps;
    Size video_size;
    string filename;
    int width_video_size, height_video_size; //用系统时间取名，防止同名文件自我覆盖
    VideoWriter src_video, fin_video;
    ofstream filterData; // 记录装甲数据输出为csv文件，方便建模分析
    ArmorDetector armor_detector;
    TargetData armor_data;
    CarData carData;

public:
    PictureManipulator(string path,
                       double fps = 120,
                       int width_video_size = 640,
                       int height_video_size = 480);
    PictureManipulator();
    ~PictureManipulator();

    virtual int manipulatePicture(Mat picture);
};

#endif