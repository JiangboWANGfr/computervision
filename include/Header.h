/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:33:58 +0800
 * @LastEditTime: 2019-09-09 22:45:36 +0800
 * @LastEditors: 
 * @Description: 
 */
#ifndef HEADER_H
#define HEADER_H

#include "config.h"
#include "actions.h"

//////////////C/C++//////////////
#include <stdio.h>
#include <algorithm>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
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

//////////////OpenCV///////////////
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

//////////namespace/////////////
using namespace cv;
using namespace std;

/////////////全局变量区//////////////
typedef struct ArmorData
{
    bool isGet; //main<-ArmorDetector，ArmorData是否清零
    bool shoot;
    float angleYaw;   //AngleCalculate->ArmorDetector->main->Serial
    float anglePitch; //AngleCalculate->ArmorDetector->main->Serial
    float x;
    float y;
    float z;
    float atocDistance; //ArmorDetector<-AngleCalculate，装甲板决策，main<-ArmorDetector
    bool isBig;         //ArmorDetector->AngleCalculate，计算pnp
    struct ArmorData operator=(const struct ArmorData &a)
    {
        isGet = a.isGet;
        shoot = a.isGet;
        angleYaw = a.angleYaw;
        anglePitch = a.anglePitch;
        x = a.x;
        y = a.y;
        z = a.z;
        atocDistance = a.atocDistance;
        isBig = a.isBig;
    }

} ArmorData; //armorData(mian)<->armordeta(ArmorDetector)<->point4Data(AngleCalculate)

typedef struct CarData
{
    float absAngleYaw;
    float absAnglePitch;
} CarData;

#endif
