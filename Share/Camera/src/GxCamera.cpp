/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-27 19:54:06 +0800
 * @LastEditTime: 2019-11-17 15:22:20 +0800
 * @LastEditors: 
 * @Description: 
 */
#include "GxCamera.h"

#ifdef GX_CAMERA

GxCamera::GxCamera(string camera_num) : Camera(camera_num)
{
}

GxCamera::~GxCamera()
{
    free(g_frame_data.pImgBuf);
}
/**
     * @Author: 王占坤
     * @Description: 打开相机
     * @Param: 
     * @Return: 
     * @Throw: 
     */
int GxCamera::open()
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

    if (setDeviceToContinuouslyAcquiredImage() == false)
        return 0;

    if (setTRiggerSwitchToOff() == false)
        return 0;

    is_opened = true;
}

/**
 * @Author: 王占坤
 * @Description: 初始化设备打开参数open_param
 * @Param: 
 * @Return: 
 * @Throw: 
 */
int GxCamera::initializeCameraDevice()
{
    //初始化设备打开参数，默认打开序号为1的设备
    open_param.accessMode = GX_ACCESS_EXCLUSIVE;
    open_param.openMode = GX_OPEN_INDEX;
    open_param.pszContent = new char[10];
    strcpy(open_param.pszContent, cam_name.c_str());
    // cout << open_param.pszContent << endl;
    // open_param.pszContent = cam_name.data();
    // open_param.pszContent = "1";
    //初始化库
    status = GXInitLib();

    cout << "GxCamera::initializeCameraDevice    初始化相机参数" << endl;

    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
        return false;
    }

    cout << "GxCamera::initializeCameraDevice    成功初始化相机参数" << endl;
    return true;
}

bool GxCamera::countNumberOfCameras()
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

bool GxCamera::openFirstCamera()
{
    status = GXOpenDevice(&open_param, &camera_handle);
    cout << "camera_handle: " << camera_handle << endl;
    GetErrorString(status);
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

/**
 * @Author: 王占坤
 * @Description: 设置感兴趣区域
 * @Param: 
 * @Return: 
 * @Throw: 
 */
bool GxCamera::setORI()
{

    //设置roi区域，设置时相机必须时停采状态
    status = GXSetInt(camera_handle, GX_INT_WIDTH, m_roi_width);
    status = GXSetInt(camera_handle, GX_INT_HEIGHT, m_roi_height);
    status = GXSetInt(camera_handle, GX_INT_OFFSET_X, m_roi_offset_x);
    status = GXSetInt(camera_handle, GX_INT_OFFSET_Y, m_roi_offset_y);
    status = GXSetFloat(camera_handle, GX_FLOAT_EXPOSURE_TIME, m_exposure_time);
    status = GXSetInt(camera_handle, GX_INT_GAIN, m_gain);

    return true;
}

/**
 * @Author: 王占坤
 * @Description: 设置采集模式为连续采集
 * @Param: 
 * @Return: 
 * @Throw: 
 */
bool GxCamera::setDeviceToContinuouslyAcquiredImage()
{
    //设置采集模式为连续采集
    status = GXSetEnum(camera_handle, GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
    if (status != GX_STATUS_SUCCESS)
    {
        // GetErrorString(status);
        // status = GXCloseDevice(camera_handle);
        // if (camera_handle != NULL)
        // {
        //     camera_handle = NULL;
        // }
        // status = GXCloseLib();
        close();
        return false;
    }
    return true;
}

bool GxCamera::setTRiggerSwitchToOff()
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

/**
     * @Author: 王占坤
     * @Description: 对相机的每一帧图像进行配置
     * @Param: int64_t width  图像横向大小
     * @Param: int54_t height 图像纵向大小
     * @Param: int offset_x 截取图像的偏置
     * @Param: int offset_y  截取图像的偏置
     * @Param: dpuuble expotime 曝光时间
     * @Param: int64_t gain 增益
     * @Return: 
     * @Throw: 
     */
int GxCamera::configFrame(int64_t width,
                        int64_t height,
                        int offset_x,
                        int offset_y,
                        double expotime,
                        int64_t gain)
{
    cout << "ConfigFrame" << endl;

    m_roi_width = width;
    m_roi_height = height;
    m_roi_offset_x = offset_x;
    m_roi_offset_y = offset_y;
    m_exposure_time = expotime;
    m_gain = gain;
    setORI();

    cout << "width = " << width << '\t' << "height = " << height << endl;
    cout << "expotime = " << expotime << '\t' << "gain = " << gain << endl;
    // 查询当前相机是否支持GX_ENUM_PIXEL_COLOR_FILTER
    status = GXIsImplemented(camera_handle, GX_ENUM_PIXEL_COLOR_FILTER, &is_colorful);
    //支持彩色图像
    if (is_colorful)
    {
        cout << "GxCamera::configFrame    支持彩色" << endl;
        status = GXGetEnum(camera_handle, GX_ENUM_PIXEL_COLOR_FILTER, &m_pixel_color);
        source_image_directly_from_camera.create(height, width, CV_8UC3);
        m_rgb_image = new char[width * height * 3];
    }
    else
    {
        cout << "GxCamera::configFrame    不支持彩色" << endl;
        source_image_directly_from_camera.create(height, width, CV_8UC1);
    }

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

int GxCamera::mallocForSourceImage()
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

/**
     * @Author: 王占坤
     * @Description: 关闭相机
     * @Param: 
     * @Return: 
     * @Throw: 
     */
int GxCamera::close()
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

/**
     * @Author: 王占坤
     * @Description: 向相机发送开始获取图像命令
     * @Param: 
     * @Return: 
     * @Throw: 
     */
int GxCamera::start()
{
    cout << "GxCamera::start    GxCamera_handle:  " << camera_handle << endl;
    status = GXSendCommand(camera_handle, GX_COMMAND_ACQUISITION_START);
    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
        return -1;
    }
    cout << "GxCamera::start    Success to start camera" << endl;

    return 0;
}

/**
     * @Author: 王占坤
     * @Description: 获取图像
     * @Param: 
     * @Return: Mat
     * @Throw: 
     */
Mat GxCamera::getFrame()
{
    if (g_frame_data.pImgBuf == NULL)
    {
        cout << "GxCamera::getFrame g_frame_data.pImgBuf == NULL" << endl;
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
                // cout << "Copy colorful image" << endl;
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


#endif