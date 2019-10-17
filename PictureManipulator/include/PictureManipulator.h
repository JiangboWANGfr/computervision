/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 11:40:32 +0800
 * @LastEditTime: 2019-10-17 23:21:02 +0800
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
public:
#ifdef SENTRY
    SentryArmorDetector armor_detector;
#endif
#ifdef INFANTRY
    InfantryArmorDetector armor_detector;
#endif

    TargetData armor_data;
    CarData car_data;

public:
    PictureManipulator(string serial_port_device,
                       string path,
                       double fps = 120,
                       int width_video_size = 640,
                       int height_video_size = 480);
    PictureManipulator() {}
    ~PictureManipulator() {}

    virtual int manipulatePicture(Mat picture) = 0;
};

#endif