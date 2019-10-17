/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 11:40:32 +0800
 * @LastEditTime: 2019-10-17 19:35:28 +0800
 * @LastEditors: 
 * @Description: 
 */
#ifndef __PICTURE_MANIPULATOR_H__
#define __PICTURE_MANIPULATOR_H__
#include "header.h"
#include "ArmorDetector.h"
#include "SerialPort.hpp"
#include "SentryArmorDetector.h"
#include "InfantryArmorDetector.h"

class PictureManipulator
{
protected:
    double fps;
    Size video_size;
    string filename;
    int width_video_size, height_video_size; //用系统时间取名，防止同名文件自我覆盖
    VideoWriter src_video, fin_video;
    ofstream filterData; // 记录装甲数据输出为csv文件，方便建模分析

#ifdef SENTRY
    SentryArmorDetector armor_detector;
#endif
#ifdef INFANTRY
    InfantryArmorDetector armor_detector;
#endif

    TargetData armor_data;
    CarData car_data;

    SerialPort stm32;

public:
    PictureManipulator(string serial_port_device,
                       string path,
                       double fps = 120,
                       int width_video_size = 640,
                       int height_video_size = 480);
    PictureManipulator();
    ~PictureManipulator();
    void adjustParameter();

    virtual int manipulatePicture(Mat picture) = 0;
};

#endif