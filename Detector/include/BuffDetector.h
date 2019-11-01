/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:26:02 +0800
 * @LastEditTime: 2019-10-31 00:35:17
 * @LastEditors: 
 * @Description: 
 */

#ifndef __BUFF_DETECTOR_H__
#define __BUFF_DETECTOR_H__

#define CAMERA1_FILEPATH "/home/waltpeter/BuffDetector_ZHBIT/param/camera_param/\
galaxy_0.xml"

#include "buff_detect.h"
#include "header.h"

class BuffDetectorWrapper
{
private:
    float distance = 0.0; 
    float yaw = 0.0; 
    float pitch = 0.0; 
    int command = 0;
    OtherParam other_param;
public:
    BuffDetectorWrapper(); 
    ~BuffDetectorWrapper();
    void getCenter(cv::Mat &source_img, TargetData &armor_data);
    void initializeParam(); 
    float getYaw(); 
    float getPitch(); 
    float getDistance(); 
private:
    
};

#endif
