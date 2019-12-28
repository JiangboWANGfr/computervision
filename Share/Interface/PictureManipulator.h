/*
 * @Author: your name
 * @Date: 2019-12-28 22:14:03
 * @LastEditTime : 2019-12-28 22:30:48
 * @LastEditors  : Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /ComputerVision/Share/Interface/PictureManipulator.h
 */

#ifndef __PICTURE_MANIPULATOR_H__
#define __PICTURE_MANIPULATOR_H__
#include "header.h"
#include "ArmorDetector.h"
#include "SerialPort.hpp"
#include "SentryArmorDetector.h"
#include "InfantryArmorDetector.h"
#include "HeroArmorDetector.hpp"
#include "BuffDetector.h"

class PictureManipulator
{
public:
#ifdef SENTRY
    SentryArmorDetector armor_detector;
#endif
#ifdef Hero 
    HeroArmorDetector armor_detector;
#endif
#ifdef INFANTRY
    InfantryArmorDetector armor_detector;
#endif
#ifdef BUFF
    BuffArmorDetector armor_detector;
#endif

TargetData armor_data;
public:
    PictureManipulator(string serial_port_device,
                       string path,
                       double fps = 120,
                       int width_video_size = 640,
                       int height_video_size = 480);
    PictureManipulator(){};
    ~PictureManipulator(){};
    virtual int manipulatePicture(Mat picture) = 0;
};

#endif
