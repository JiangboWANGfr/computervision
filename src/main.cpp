/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 19:35:43 +0800
 * @LastEditTime: 2019-09-13 22:09:59
 * @LastEditors: 
 * @Description: 
 */

#include "head.h"
#include "prepare_camera.h"
#include "manipulate_picture.h"
#include "ArmorDetector.hpp"
#include "AngleCalculate.hpp"
#include "Serial.hpp"
#include "RMVideoCapture.hpp"
#include "DxImageProc.h"
#include "GxIAPI.h"

/////////////////全局变量声明区//////////////
int64_t m_pixel_color = 0; ///< Bayer格式
bool is_implemented;       //是否支持彩色
GX_STATUS status;
GX_DEV_HANDLE camera_handle = NULL; ///< 相机句柄
char *m_rgb_image = NULL;
GX_FRAME_DATA g_frame_data = {0}; ///< 采集图像参数
bool g_get_image = false;         ///< 采集线程是否结束的标志：true 运行；false 退出
Mat source_image_directly_from_camera;
uint32_t ret = 0; //API接口函数返回值
#ifdef STM32
Serialport stm32("/dev/ttyUSB0");
#endif

////////////////////函数声明///////////
int configSourceImage();
int mallocForSourceImage();
bool startReceiveImageThread();
void *getImageFromCamera(void *pParam);

int UnPreForImage()
{
    GX_STATUS status = GX_STATUS_SUCCESS;
    uint32_t ret = 0;

    //发送停采命令
    status = GXSendCommand(camera_handle, GX_COMMAND_ACQUISITION_STOP);
    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
        return status;
    }

    g_get_image = false;
    // ret = pthread_join(g_acquire_thread, NULL);
    if (ret != 0)
    {
        printf("<Failed to release resources>\n");
        return ret;
    }

    //释放buffer
    if (g_frame_data.pImgBuf != NULL)
    {
        free(g_frame_data.pImgBuf);
        g_frame_data.pImgBuf = NULL;
    }

    return 0;
}

int closeAll()
{
    //为停止采集做准备
    ret = UnPreForImage();
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

///////////////////main///////////////

int main()
{
    cout << "Initializion.........." << endl;
    prepareCamera(camera_handle);
    cout << "camera_handle: " << camera_handle << endl;
    cout << "Success to open camera" << endl;
    configSourceImage();
    cout << "Success to config source Image" << endl;
    //为采集做准备
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
        return 0;
    }
    if (startReceiveImageThread() == false)
        return 0;

    mainpulatePicture(source_image_directly_from_camera);

    closeAll();
}

////////////////////函数实现////////////////////////

int configSourceImage()
{
    int64_t width = 0, height = 0;
    double expotime = 0;
    int64_t gain = 0;
    status = GXGetInt(camera_handle, GX_INT_WIDTH, &width);
    status = GXGetInt(camera_handle, GX_INT_HEIGHT, &height);
    status = GXGetFloat(camera_handle, GX_FLOAT_EXPOSURE_TIME, &expotime);
    status = GXGetInt(camera_handle, GX_INT_GAIN, &gain);
    cout << "width = " << width << '\t' << "height = " << height << endl;
    cout << "expotime = " << expotime << '\t' << "gain = " << gain << endl;
    // 查询当前相机是否支持GX_ENUM_PIXEL_COLOR_FILTER
    status = GXIsImplemented(camera_handle, GX_ENUM_PIXEL_COLOR_FILTER, &is_implemented);
    //支持彩色图像
    if (is_implemented)
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
}

int mallocForSourceImage()
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

bool startReceiveImageThread()
{
    //启动接收线程
    pthread_t tid;
    int ret = pthread_create(&tid, 0, getImageFromCamera, 0);
    if (ret != 0)
    {
        printf("<Failed to create the collection thread>\n");
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

void *getImageFromCamera(void *pParam)
{
    pthread_detach(pthread_self());
    GX_STATUS status = GX_STATUS_SUCCESS;
    //接收线程启动标志
    g_get_image = true;
    //发送开采命令
    status = GXSendCommand(camera_handle, GX_COMMAND_ACQUISITION_START);
    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
    }
    while (g_get_image)
    {
        if (g_frame_data.pImgBuf == NULL)
        {
            continue;
        }
        status = GXGetImage(camera_handle, &g_frame_data, 100);
        if (status != GX_STATUS_SUCCESS)
        {
            continue;
        }
        else
        {
            if (g_frame_data.nStatus == GX_FRAME_STATUS_SUCCESS)
            {
                // printf("<Successful acquisition : Width: %d Height: %d >\n", g_frame_data.nWidth, g_frame_data.nHeight);
                if (is_implemented)
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
    }
}
