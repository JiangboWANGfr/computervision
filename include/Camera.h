/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-27 19:52:54 +0800
 * @LastEditTime: 2019-09-27 19:57:16 +0800
 * @LastEditors: 
 * @Description: 
 */
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "header.h"
#include "actions.h"

class Camera
{
private:
    GX_OPEN_PARAM open_param;
    GX_STATUS status;

    int64_t m_roi_width = 640;       ///< 感兴趣区域宽
    int64_t m_roi_height = 480;      ///< 感兴趣区域高
    int64_t m_roi_offset_x = 8;      ///< 水平偏移量设置     //8 //320
    int64_t m_roi_offset_y = 6;      ///< 竖直偏移量设置     //6 //272
    double m_exposure_time = 1200.0; ///< 曝光时间     识别数字时为2000
    int64_t m_gain = 200;            ///< 增益       识别数字时为850,分区赛用的500，复活赛用的200

    uint32_t camera_num;

public:
    GX_DEV_HANDLE camera_handle = NULL; ///< 相机句柄

private:
    int initializeCameraDevice();
    bool countNumberOfCameras();
    bool openFirstCamera();
    bool setORI();
    bool setDeviceToContinuouslyAcquiredImage();
    bool setTRiggerSwitchToOff();

public:
    Camera(/* args */);
    ~Camera();
    int prepareCamera();
};


#endif