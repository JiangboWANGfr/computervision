/*
 * @Description: In User Settings Edit
 * @Author:
 * @Date: 2019-09-13 19:49:23
 * @LastEditTime: 2019-09-25 16:22:25 +0800
 * @LastEditors: Please set LastEditors
 */
#include "manipulate_picture.h"
#include "Serial.hpp"
#include "ArmorDetector.hpp"

#ifdef STM32
SerialPort stm32("/dev/ttyUSB0");
#endif

int mode = 0;

// 视频记录，videorate值有待商榷
double videoRate = 60.0;
Size videoSize(640, 480);
string filename = "../build/" + std::to_string(time(NULL));                                      //用系统时间取名，防止同名文件自我覆盖
VideoWriter videoSrc(filename + "Src.avi", CV_FOURCC('M', 'J', 'P', 'G'), videoRate, videoSize); //记录原始图，用于调参使用
VideoWriter videoFin(filename + "Fin.avi", CV_FOURCC('M', 'J', 'P', 'G'), videoRate, videoSize); //记录结果图，用于判定赛场表现
ofstream filterData;                                                                             // 记录装甲数据输出为csv文件，方便建模分析
ArmorDetector armor_detector;
TargetData armor_data;
CarData carData;

void sentryMainpulatePicture(Mat &source_image_directly_from_camera);

void heroManipulatePicture(Mat &source_image_directly_from_camera);

void infantryManipulatePicture(Mat &source_image_directly_from_camera);

void UAVManipulatePicture(Mat &source_image_directly_from_camera);

void initArmorData(TargetData &armorData);

void lostTargetShortTime_toDo(bool is_insight);

void lostTargetLongTime_toDo(bool is_insight);

void maybeLostTarget(unsigned int lost, bool is_insight);

void writeIntoFilterDataCSV();

int judgeTargetInsight(unsigned int &lost, bool &is_insight);

int mainpulatePicture(Mat &source_image_directly_from_camera)
{
    if (corps == SENTRY)
    {
        sentryMainpulatePicture(source_image_directly_from_camera);
    }
    else if (corps == HERO)
    {
        heroManipulatePicture(source_image_directly_from_camera);
    }
    else if (corps == INFANTRY)
    {
        infantryManipulatePicture(source_image_directly_from_camera);
    }
    else
    {
        UAVManipulatePicture(source_image_directly_from_camera);
    }
}

void sentryManipulatePicture(Mat &source_image_directly_from_camera)
{
    initArmorData(armor_data);
    Mat img;
    unsigned int lost = 0;
    bool is_insight = false;

    while (true)
    {
        img = source_image_directly_from_camera.clone();
#ifdef STM32
        stm32.readMode(mode); //步兵
#endif
        double start = clock();
        videoSrc << img; //保存图片
        armor_detector.getCenters(img, armor_data);
        videoFin << img; //保存处理后的图片

        //防止计数溢出
        lost = lost < 2000 ? lost : 2000;

        //目标丢失与否
        judgeTargetInsight(lost, is_insight);

        maybeLostTarget(lost, is_insight);

        //敌人不在视野中，初始化数据
        if (is_insight == false)
        {
            initArmorData(armor_data);
        }

#ifdef STM32
        //步兵
        stm32.sendAngleDist(armor_data.yaw_angle, armor_data.pitch_angle, armor_data.atocDistance, is_insight);

        //串口通信发送信息给电控stm32,哨兵
        stm32.sendAngle(armor_data.yaw_angle, armor_data.pitch_angle, armor_data.atocDistance,
                        armor_data.is_big, is_insight, armor_data.is_get);
#endif

        writeIntoFilterDataCSV();

#ifdef DEBUG1
        showPicture("armor", img, 2);
        imshow("armor", img);
#endif
        //没有waitkey()，imshow()会出现问题，但是似乎并不影响识别，待测试
        imshow("hello", source_image_directly_from_camera);
        waitKey(1); //即使没有imshow也要有waitKey，否则出现锁不住跟不上的bug

        double end = clock();
        std::cout << "Time Per Pic:" << (end - start) / CLOCKS_PER_SEC * 1000 << "ms\n"
                  << endl;
    }
}

void initArmorData(TargetData &armorData)
{
    // 初始化armorData
    armorData.yaw_angle = 0.0f;
    armorData.pitch_angle = 0.0f;
    armorData.atocDistance = 0.0f;
    armorData.is_big = false;
    armorData.shoot = false;
}

void lostTargetShortTime_toDo(bool is_insight)
{
    // cout << "lost = " << lost << endl;
    is_insight = true;
    armor_data.yaw_angle = armor_data.yaw_angle * 0; //复活赛抛弃识别数字，对图像质量要求降低，识别算法很少丢帧故去掉滤��    armor_data.pitch_angle = armor_data.pitch_angle * 0;
    // armor_data.yaw_angle = abs(armor_data.yaw_angle) > 0.01f ? armor_data.yaw_angle : 0.0f;
    // armor_data.pitch_angle = abs(armor_data.pitch_angle) > 0.01f ? armor_data.pitch_angle : 0.0f;
}

void lostTargetLongTime_toDo(bool is_insight)
{
    is_insight = true;
    // armor_data.yaw_angle = 0.0f;
    // armor_data.pitch_angle = 0.0f;
}

void maybeLostTarget(unsigned int lost, bool is_insight)
{
    //丢失目标后的短暂寻找策略
    int lostmin = 30;
    int lostmax = 100; //根据自己的算法所需时间让云台停1.5s左右,复活赛时哨兵巡逻yaw��s转一圈，pitch轴搜索范��-30度，一圈俯����    if (lost < lostmin && lost > 0) //丢失时间很短，处理云台自身快速转动丢失目标问��    {
    lostTargetShortTime_toDo(&is_insight);

    if (lost >= lostmin && lost < lostmax) //丢失时间较短，处理猫步或陀螺切换装甲板导致目标短暂消失问题
    {
        lostTargetLongTime_toDo(&is_insight);
    }
    if (lost >= lostmax) //目标可能的确走了，即不在视野中，而不是因为没识别��
    {
        is_insight = false;
    }
}

void writeIntoFilterDataCSV()
{
    //输出数据到csv文件，方便赛后检查数据是否异常，因为视频太快，其中某帧出问题的话，人可能看不出来有问题
    filterData << (double)clock() / CLOCKS_PER_SEC << ","
               << armor_data.yaw_angle << ","
               << armor_data.pitch_angle << ","
               << armor_data.atocDistance << endl;
}

int judgeTargetInsight(unsigned int &lost, bool &is_insight)
{
    if (armor_data.is_get == true)
    {
        lost = 0;
        is_insight = true;
    }
    else
    {
        lost++;
    }
}
