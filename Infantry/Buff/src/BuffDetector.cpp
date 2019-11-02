/*
 * @Author: your name
 * @Date: 2019-10-30 17:25:08
 * @LastEditTime: 2019-10-31 00:35:00
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MergeBuff/Detector/src/BuffDetector.cpp
 */
#include "buff_detect.h"
#include "BuffDetector.h"

void BuffDetectorWrapper::getCenter(Mat &img, TargetData &armor_data) {
    BuffDetector buff_detector; 
    command = buff_detector.BuffDetectTask(img, other_param); 
    if(command)
    {
        buff_detector.getAngle(yaw, pitch);
        distance = buff_detector.getDistance();
        printf("Distance: %f, Yaw: %f, Pitch: %f\n", distance, yaw, pitch); 
        armor_data.yaw_angle = yaw; 
        armor_data.pitch_angle = pitch; 
        armor_data.atocDistance = distance; 
    }
}

void BuffDetectorWrapper::initializeParam() {
    BuffDetector buff_detector; 
    buff_detector.readXML(); 
}

float BuffDetectorWrapper::getYaw() {
    return yaw; 
}

float BuffDetectorWrapper::getPitch() {
    return pitch; 
}

float BuffDetectorWrapper::getDistance() { 
    return distance; 
}