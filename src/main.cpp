/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 19:35:43 +0800
 * @LastEditTime: 2019-09-11 17:54:35 +0800
 * @LastEditors: 
 * @Description: 
 */

#include "head.h"
#include "prepare_camera.h"

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

//API接口函数返回值
uint32_t ret = 0;

Mat ImgDown;
#ifdef STM32
Serialport stm32("/dev/ttyUSB0");
#endif
ArmorDetector armor_detector;
ArmorData armor_data;
CarData carData;

pthread_t idSA;

// 视频记录，videorate值有待商榷
double videoRate = 60.0;
Size videoSize(640, 480);
string filename = "../build/" + std::to_string(time(NULL));                                      //用系统时间取名，防止同名文件自我覆盖
VideoWriter videoSrc(filename + "Src.avi", CV_FOURCC('M', 'J', 'P', 'G'), videoRate, videoSize); //记录原始图，用于调参使用
VideoWriter videoFin(filename + "Fin.avi", CV_FOURCC('M', 'J', 'P', 'G'), videoRate, videoSize); //记录结果图，用于判定赛场表现
ofstream filterData;                                                                             // 记录装甲数据输出为csv文件，方便建模分析

////////////////////函数声明///////////
int configSourceImage();
int mallocForSourceImage();
bool startReceiveImageThread();
void *getImageFromCamera(void *pParam);

void initArmorData(ArmorData &armorData)
{
    // 初始化armorData
    armorData.angleYaw = 0.0f;
    armorData.anglePitch = 0.0f;
    armorData.atocDistance = 0.0f;
    armorData.isBig = false;
    armorData.shoot = false;
}

void lostTargetShortTime_toDo(bool is_insight)
{
    // cout << "lost = " << lost << endl;
    is_insight = true;
    armor_data.angleYaw = armor_data.angleYaw * 0; //复活赛抛弃识别数字，对图像质量要求降低，识别算法很少丢帧故去掉滤波
    armor_data.anglePitch = armor_data.anglePitch * 0;
    // armor_data.angleYaw = abs(armor_data.angleYaw) > 0.01f ? armor_data.angleYaw : 0.0f;
    // armor_data.anglePitch = abs(armor_data.anglePitch) > 0.01f ? armor_data.anglePitch : 0.0f;
}

void lostTargetLongTime_toDo(bool is_insight)
{
    is_insight = true;
    // armor_data.angleYaw = 0.0f;
    // armor_data.anglePitch = 0.0f;
}

void maybeLostTarget(unsigned int lost, bool is_insight)
{
    //丢失目标后的短暂寻找策略
    int lostmin = 30;
    int lostmax = 100;              //根据自己的算法所需时间让云台停1.5s左右,复活赛时哨兵巡逻yaw轴3s转一圈，pitch轴搜索范围5-30度，一圈俯仰4次
    if (lost < lostmin && lost > 0) //丢失时间很短，处理云台自身快速转动丢失目标问题
    {
        lostTargetShortTime_toDo(&is_insight);
    }
    if (lost >= lostmin && lost < lostmax) //丢失时间较短，处理猫步或陀螺切换装甲板导致目标短暂消失问题
    {
        lostTargetLongTime_toDo(&is_insight);
    }
    if (lost >= lostmax) //目标可能的确走了，即不在视野中，而不是因为没识别到
    {
        is_insight = false;
    }
}

void writeIntoFilterDataCSV()
{
    //输出数据到csv文件，方便赛后检查数据是否异常，因为视频太快，其中某帧出问题的话，人可能看不出来有问题
    filterData << (double)clock() / CLOCKS_PER_SEC << ","
               << armor_data.angleYaw << ","
               << armor_data.anglePitch << ","
               << armor_data.atocDistance << endl;
}

int judgeTargetInsight(unsigned int &lost, bool &is_insight)
{
    if (armor_data.isGet == true)
    {
        lost = 0;
        is_insight = true;
    }
    else
    {
        lost++;
    }
}

void kernel()
{
    initArmorData(armor_data);
    Mat ImgDown = source_image_directly_from_camera.clone();
    unsigned int lost = 0;
    bool is_insight = false;

    while (!ImgDown.empty())
    {
        double start = clock();
        videoSrc << ImgDown; //保存图片
        armor_detector.getCenters(ImgDown, armor_data);
        videoFin << ImgDown; //保存处理后的图片

        //防止计数溢出
        lost = lost < 2000 ? lost : 2000;

        //目标丢失与否
        judgeTargetInsight(lost, is_insight);

        maybeLostTarget(lost, is_insight);

        //防止云台疯转，根据视场角实测得到两轴角度范围
        armor_data.angleYaw = abs(armor_data.angleYaw) < 12.0f ? armor_data.angleYaw : 0.0f;
        armor_data.anglePitch = abs(armor_data.anglePitch) < 12.0f ? armor_data.anglePitch : 0.0f;
        //敌人不在视野中，初始化数据
        if (is_insight == false)
        {
            initArmorData(armor_data);
        }

#ifdef STM32
        //串口通信发送信息给电控stm32
        stm32.sendAngle(armor_data.angleYaw, armor_data.anglePitch, armor_data.atocDistance,
                        armor_data.isBig, isInsight, armor_data.isGet);
#endif

        writeIntoFilterDataCSV();

#ifdef DEBUG1
        showPicture("armor", ImgDown, 2);
        imshow("armor", ImgDown);
#endif
        //没有waitkey()，imshow()会出现问题，但是似乎并不影响识别，待测试
        waitKey(1); //即使没有imshow也要有waitKey，否则出现锁不住跟不上的bug
        ImgDown = source_image_directly_from_camera.clone();

        double end = clock();
        cout << "Time Per Pic:" << (end - start) / CLOCKS_PER_SEC * 1000 << "ms\n"
             << endl;
    }
}

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
    cout << "camera_handle: "<< camera_handle << endl;    
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

    kernel();
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
