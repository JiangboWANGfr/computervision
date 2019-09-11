/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 11:30:27 +0800
 * @LastEditTime: 2019-09-09 23:02:33 +0800
 * @LastEditors: 
 * @Description: 
 */
#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include "head.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
using namespace std;
using namespace cv;


void showPicture(string name, const Mat& picture, double time);

void GetErrorString(GX_STATUS error_status);




#endif