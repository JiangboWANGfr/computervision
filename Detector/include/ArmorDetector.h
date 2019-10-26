/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:26:02 +0800
 * @LastEditTime: 2019-10-17 19:38:08 +0800
 * @LastEditors: 
 * @Description: 
 */
#ifndef __ARMOR_DETECTOR_H__
#define __ARMOR_DETECTOR_H__

#include "header.h"

class ArmorDetector
{
public:
    int gray_thresh = 100;
    int single_color_thresh = 150;
    int mode = 1;   //步兵
    int isred = 1;  //步兵
    int islost = 1; //步兵
    TargetData armor_data;

#ifdef BLUE
    cv::Scalar huahuaScalar = cv::Scalar(255, 0, 0);
#endif
#ifdef RED
    cv::Scalar huahuaScalar = cv::Scalar(0, 0, 255);
#endif

public:
    ArmorDetector(){}
   ~ArmorDetector(){}
    virtual void getCenter(cv::Mat &source_img, TargetData &armor_data) = 0;
};

#endif
