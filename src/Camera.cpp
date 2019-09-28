/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-27 19:54:06 +0800
 * @LastEditTime: 2019-09-28 14:18:58 +0800
 * @LastEditors: 
 * @Description: 
 */
#include "Camera.h"
Camera::Camera(/* args */)
{
}

Camera::~Camera()
{
}

int Camera::open()
{
    if (initializeCameraDevice() == false)
        return 0;

    if (countNumberOfCameras() == false)
        return 0;
    if (camera_num <= 0)
    {
        printf("<No device>\n");
        status = GXCloseLib();
        cout << "<Closed Device>" << endl;
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
    
    is_opened = true;
    cout << "camera_handle: " << camera_handle << endl;
    cout << "Success to open camera" << endl;
}

int Camera::initializeCameraDevice()
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

bool Camera::countNumberOfCameras()
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

bool Camera::openFirstCamera()
{
    status = GXOpenDevice(&open_param, &camera_handle);
    cout << "camera_handle: " << camera_handle << endl;
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

bool Camera::setORI()
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

bool Camera::setDeviceToContinuouslyAcquiredImage()
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

bool Camera::setTRiggerSwitchToOff()
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

int Camera::configFrame()
{
    int64_t width = 0, height = 0;
    double expotime = 0;
    int64_t gain = 0;
    cout << "ConfigFrame" << endl;
    GX_STATUS status = GXGetInt(camera_handle, GX_INT_WIDTH, &width);
    status = GXGetInt(camera_handle, GX_INT_HEIGHT, &height);
    status = GXGetFloat(camera_handle, GX_FLOAT_EXPOSURE_TIME, &expotime);
    status = GXGetInt(camera_handle, GX_INT_GAIN, &gain);
    cout << "width = " << width << '\t' << "height = " << height << endl;
    cout << "expotime = " << expotime << '\t' << "gain = " << gain << endl;
    // 查询当前相机是否支持GX_ENUM_PIXEL_COLOR_FILTER
    status = GXIsImplemented(camera_handle, GX_ENUM_PIXEL_COLOR_FILTER, &is_colorful);
    //支持彩色图像
    if (is_colorful)
    {
        cout << "支持彩色" << endl;
        status = GXGetEnum(camera_handle, GX_ENUM_PIXEL_COLOR_FILTER, &m_pixel_color);
        source_image_directly_from_camera.create(height, width, CV_8UC3);
        m_rgb_image = new char[width * height * 3];
    }
    else
    {
        cout << "不支持彩色" << endl;
        source_image_directly_from_camera.create(height, width, CV_8UC1);
    }

    mallocForSourceImage();
    int ret = mallocForSourceImage();
    if (ret != 0)
    {
        printf("<Failed to prepare for acquire image>\n");
        status = GXCloseDevice(camera_handle);
        if (camera_handle != NULL)
        {
            camera_handle = NULL;
        }
        status = GXCloseLib();
        return -1;
    }
}

int Camera::mallocForSourceImage()
{
    GX_STATUS status = GX_STATUS_SUCCESS;
    int64_t payload_size = 0;

    status = GXGetInt(camera_handle, GX_INT_PAYLOAD_SIZE, &payload_size);
    cout << "payload_size = " << payload_size << endl;
    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
        return status;
    }

    g_frame_data.pImgBuf = malloc(payload_size);
    if (g_frame_data.pImgBuf == NULL)
    {
        printf("<Failed to allot memory>\n");
        return 0;
    }

    return 0;
}

int Camera::close()
{
    //为停止采集做准备

    GX_STATUS status = GX_STATUS_SUCCESS;
    uint32_t ret = 0;

    //发送停采命令
    status = GXSendCommand(camera_handle, GX_COMMAND_ACQUISITION_STOP);
    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
        return status;
    }

    // g_get_image = false;
    // // ret = pthread_join(g_acquire_thread, NULL);
    // if (ret != 0)
    // {
    //     printf("<Failed to release resources>\n");
    //     return ret;
    // }

    // //释放buffer
    // if (g_frame_data.pImgBuf != NULL)
    // {
    //     free(g_frame_data.pImgBuf);
    //     g_frame_data.pImgBuf = NULL;
    // }

    //关闭设备
    status = GXCloseDevice(camera_handle);
    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
        if (camera_handle != NULL)
        {
            camera_handle = NULL;
        }
        status = GXCloseLib();
        return 0;
    }
    //释放库
    else
        status = GXCloseLib();

    return 0;
}

int Camera::start()
{
    status = GXSendCommand(camera_handle, GX_COMMAND_ACQUISITION_START);
    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
    }
}

Mat Camera::getFrame()
{
    if (g_frame_data.pImgBuf == NULL)
    {
        // to do
    }
    status = GXGetImage(camera_handle, &g_frame_data, 100);
    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
    }
    else
    {
        if (g_frame_data.nStatus == GX_FRAME_STATUS_SUCCESS)
        {
            // printf("<Successful acquisition : Width: %d Height: %d >\n", g_frame_data.nWidth, g_frame_data.nHeight);
            if (is_colorful)
            {
                DxRaw8toRGB24(g_frame_data.pImgBuf, m_rgb_image, g_frame_data.nWidth, g_frame_data.nHeight,
                              RAW2RGB_NEIGHBOUR, DX_PIXEL_COLOR_FILTER(BAYERBG), false);
                memcpy(source_image_directly_from_camera.data, m_rgb_image, g_frame_data.nHeight * g_frame_data.nWidth * 3);
            }
            else
            {
                memcpy(source_image_directly_from_camera.data, g_frame_data.pImgBuf, g_frame_data.nHeight * g_frame_data.nWidth);
            }
        }
    }
    return source_image_directly_from_camera.clone();
}