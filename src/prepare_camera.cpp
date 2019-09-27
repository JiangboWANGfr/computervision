/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 19:44:43 +0800
 * @LastEditTime: 2019-09-09 23:02:16 +0800
 * @LastEditors: 
 * @Description: 这里是对相机进行配置
 */
#include "header.h"
#include "prepare_camera.h"

/////////////////////文件内部全局变量声明区//////////////////////////
static GX_DEV_HANDLE camera_handle = NULL; ///< 相机句柄
GX_OPEN_PARAM open_param;
static GX_STATUS status;


int64_t m_roi_width = 640;       ///< 感兴趣区域宽
int64_t m_roi_height = 480;      ///< 感兴趣区域高
int64_t m_roi_offset_x = 8;      ///< 水平偏移量设置     //8 //320
int64_t m_roi_offset_y = 6;      ///< 竖直偏移量设置     //6 //272
double m_exposure_time = 1200.0; ///< 曝光时间     识别数字时为2000
int64_t m_gain = 200;            ///< 增益       识别数字时为850,分区赛用的500，复活赛用的200

uint32_t camera_num;

//////////////////局部变量声明////////////////

int initializeCameraDevice();
bool countNumberOfCameras();
bool openFirstCamera();
bool setORI();
bool setDeviceToContinuouslyAcquiredImage();
bool setTRiggerSwitchToOff();

//////////////////////函数实现//////////////////

int prepareCamera(GX_DEV_HANDLE &camera_hd)
{
    if (initializeCameraDevice() == false)
        return 0;

    if (countNumberOfCameras() == false)
        return 0;

    if (camera_num <= 0)
    {
        printf("<No device>\n");
        status = GXCloseLib();
        return 0;
    }

    //默认打开第1个设备
    if (openFirstCamera() == false)
        return 0;
    if (setORI() == false)
    {
        return 0;
    }

    if (setDeviceToContinuouslyAcquiredImage() == false)
        return 0;

    if (setTRiggerSwitchToOff() == false)
        return 0;

    camera_hd = camera_handle;
}


int initializeCameraDevice()
{
    //初始化设备打开参数，默认打开序号为1的设备
    open_param.accessMode = GX_ACCESS_EXCLUSIVE;
    open_param.openMode = GX_OPEN_INDEX;
    open_param.pszContent = "1";
    //初始化库
    status = GXInitLib();

    cout << "初始化相机参数" << endl;

    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
        return false;
    }

    cout << "成功初始化相机参数" << endl;
    return true;
}

bool countNumberOfCameras()
{
    //获取枚举设备个数
    status = GXUpdateDeviceList(&camera_num, 1000);
    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
        status = GXCloseLib();
        return false;
    }
    cout << "number of cameras: " << camera_num << endl;
    return true;
}

bool openFirstCamera()
{
    status = GXOpenDevice(&open_param, &camera_handle);
    cout << "camera_handle: "<< camera_handle << endl;
    if (status != GX_STATUS_SUCCESS)
    {
        printf("Failed to open camera.\n");
        status = GXCloseLib();
        return false;
    }
    else
    {
        printf("Success to open camera.\n");
    }
    return true;
}

bool setORI()
{
    
    bool b_is_set_roi = true;
    //设置roi区域，设置时相机必须时停采状态
    if (b_is_set_roi)
    {
        status = GXSetInt(camera_handle, GX_INT_WIDTH, m_roi_width);
        status = GXSetInt(camera_handle, GX_INT_HEIGHT, m_roi_height);
        status = GXSetInt(camera_handle, GX_INT_OFFSET_X, m_roi_offset_x);
        status = GXSetInt(camera_handle, GX_INT_OFFSET_Y, m_roi_offset_y);
        status = GXSetFloat(camera_handle, GX_FLOAT_EXPOSURE_TIME, m_exposure_time);
        status = GXSetInt(camera_handle, GX_INT_GAIN, m_gain);
    }

    return true;
}

bool setDeviceToContinuouslyAcquiredImage()
{
    //设置采集模式为连续采集
    status = GXSetEnum(camera_handle, GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
        status = GXCloseDevice(camera_handle);
        if (camera_handle != NULL)
        {
            camera_handle = NULL;
        }
        status = GXCloseLib();
        return false;
    }
    return true;
}

bool setTRiggerSwitchToOff()
{
    //设置触发开关为OFF
    status = GXSetEnum(camera_handle, GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF);
    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
        status = GXCloseDevice(camera_handle);
        if (camera_handle != NULL)
        {
            camera_handle = NULL;
        }
        status = GXCloseLib();
        return false;
    }

    return true;
}


