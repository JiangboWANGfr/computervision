/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-25 16:52:29 +0800
 * @LastEditTime: 2019-09-25 17:18:02 +0800
 * @LastEditors: 
 * @Description: 
 */
#include "prepare_image.h"
#include "actions.h"

///////////////////全局变量声明/////////////////////
int64_t m_pixel_color = 0; ///< Bayer格式
bool is_implemented;       //是否支持彩色
GX_DEV_HANDLE camera_handle; ///< 相机句柄
bool g_get_image = false;         ///< 采集线程是否结束的标志：true 运行；false 退出
Mat source_image_directly_from_camera;
char *m_rgb_image = NULL;
GX_FRAME_DATA g_frame_data = {0}; ///< 采集图像参数
GX_STATUS status;


////////////////函数声明////////////
int mallocForSourceImage();
bool startReceiveImageThread();
void *getImageFromCamera(void *pParam);

int configSourceImage(GX_DEV_HANDLE& camera_hd)
{
    camera_handle = camera_hd;
    int64_t width = 0, height = 0;
    double expotime = 0;
    int64_t gain = 0;
    GX_STATUS status = GXGetInt(camera_handle, GX_INT_WIDTH, &width);
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
        image_queue.push(source_image_directly_from_camera.clone());
    }
}
