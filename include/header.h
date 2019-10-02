/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:33:58 +0800
 * @LastEditTime: 2019-10-01 20:30:13 -0700
 * @LastEditors: 
 * @Description: 
 */
#ifndef __HEADER_H__
#define __HEADER_H__

#include "config.h"

//////////////C/C++//////////////
#include <stdio.h>
#include <algorithm>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <thread>
#include <linux/videodev2.h>
#include <errno.h>

#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

//////////////OpenCV///////////////
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

//////////////其他//////////////
#include "GxIAPI.h"
// #include "Serialport.h"
#include "DxImageProc.h"

//////////namespace/////////////
using namespace cv;
using namespace std;

////////////////////define/////////////////////


////////////////////typedef////////////////////

///////////////////自定义数据类型//////////////////
typedef enum CORPS
{
    SENTRY = 1,   //哨兵
    INFANTRY = 2, //步兵
    HERO = 3,     //英雄
    UAV = 4       //无人机
} CORPS;

typedef struct TargetData
{
    bool is_get; //main<-ArmorDetector，ArmorData是否清零
    bool shoot;
    float yaw_angle;   //AngleCalculate->ArmorDetector->main->Serial
    float pitch_angle; //AngleCalculate->ArmorDetector->main->Serial
    float x;
    float y;
    float z;
    float atocDistance; //ArmorDetector<-AngleCalculate，装甲板决策，main<-ArmorDetector
    bool is_big;        //ArmorDetector->AngleCalculate，计算pnp
    struct TargetData operator=(const struct TargetData &a)
    {
        is_get = a.is_get;
        shoot = a.is_get;
        yaw_angle = a.yaw_angle;
        pitch_angle = a.pitch_angle;
        x = a.x;
        y = a.y;
        z = a.z;
        atocDistance = a.atocDistance;
        is_big = a.is_big;
    }

} TargetData; //armorData(mian)<->armordeta(ArmorDetector)<->point4Data(AngleCalculate)

typedef struct CarData
{
    float absAngleYaw;
    float absAnglePitch;
} CarData;

///////////////////全局变量//////////////////////

extern queue<Mat> image_queue;

#endif
