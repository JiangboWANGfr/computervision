#ifndef __GX_CAMERA_H__
#define __GX_CAMERA_H__


#include "header.h"
#include "actions.h"
#include "Camera.h"

class GxCamera : public Camera
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
    int64_t m_pixel_color = 0; ///< Bayer格式
    bool is_colorful;          //是否支持彩色
    bool g_get_image = false;  ///< 采集线程是否结束的标志：true 运行；false 退出
    Mat source_image_directly_from_camera;
    char *m_rgb_image = NULL;
    GX_FRAME_DATA g_frame_data = {0}; ///< 采集图像参数


private:
    int initializeCameraDevice();
    bool countNumberOfCameras();
    bool openFirstCamera();
    bool setORI();
    bool setDeviceToContinuouslyAcquiredImage();
    bool setTRiggerSwitchToOff();
    int mallocForSourceImage();

public:
    GxCamera(/* args */);
    ~GxCamera();
    int open();
    int configFrame(int64_t width,
                    int64_t height,
                    int offset_x,
                    int offset_y ,
                    double expotime,
                    int64_t gain);
    int close();
    int start();
    Mat getFrame();
};


#endif