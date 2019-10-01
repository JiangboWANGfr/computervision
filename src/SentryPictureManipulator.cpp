/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 13:14:07 +0800
 * @LastEditTime: 2019-09-29 07:30:32
 * @LastEditors: 
 * @Description: 
 */
#include "SentryPictureManipulator.h"

SentryPictureManipulator::SentryPictureManipulator(string serial_port_device,
                                                   string path,
                                                   double fps,
                                                   int width_video_size,
                                                   int height_video_size)
    : PictureManipulator(serial_port_device,
                         path,
                         fps,
                         width_video_size,
                         height_video_size)
{
}

SentryPictureManipulator::SentryPictureManipulator()
{
}

SentryPictureManipulator::~SentryPictureManipulator()
{
}

int SentryPictureManipulator::manipulatePicture(Mat source_image_directly_from_camera)
{

    initArmorData();
    Mat img = source_image_directly_from_camera;
    unsigned int lost = 0;
    bool is_insight = false;

    double start = clock();
    src_video << img; //保存图片
    armor_detector.getCenters(img, armor_data);
    fin_video << img; //保存处理后的图片

    //防止计数溢出
    lost = lost < 2000 ? lost : 2000;

    //目标丢失与否
    judgeTargetInsight();

    maybeLostTarget();

    //敌人不在视野中，初始化数据
    if (is_insight == false)
    {
        initArmorData();
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

    double end = clock();
    std::cout << "Time Per Pic:" << (end - start) / CLOCKS_PER_SEC * 1000 << "ms\n"
              << endl;
}

void SentryPictureManipulator::initArmorData()
{
    // 初始化armorData
    armor_data.yaw_angle = 0.0f;
    armor_data.pitch_angle = 0.0f;
    armor_data.atocDistance = 0.0f;
    armor_data.is_big = false;
    armor_data.shoot = false;
}

void SentryPictureManipulator::lostTargetShortTime_toDo()
{
    // cout << "lost = " << lost << endl;
    is_insight = true;
    armor_data.yaw_angle = armor_data.yaw_angle * 0; //复活赛抛弃识别数字，对图像质量要求降低，识别算法很少丢帧故去掉滤��    armor_data.pitch_angle = armor_data.pitch_angle * 0;
    // armor_data.yaw_angle = abs(armor_data.yaw_angle) > 0.01f ? armor_data.yaw_angle : 0.0f;
    // armor_data.pitch_angle = abs(armor_data.pitch_angle) > 0.01f ? armor_data.pitch_angle : 0.0f;
}

void SentryPictureManipulator::lostTargetLongTime_toDo()
{
    is_insight = true;
    // armor_data.yaw_angle = 0.0f;
    // armor_data.pitch_angle = 0.0f;
}

void SentryPictureManipulator::maybeLostTarget()
{
    //丢失目标后的短暂寻找策略
    int lostmin = 30;
    int lostmax = 100; //根据自己的算法所需时间让云台停1.5s左右,复活赛时哨兵巡逻yaw��s转一圈，pitch轴搜索范��-30度，一圈俯����    if (lost < lostmin && lost > 0) //丢失时间很短，处理云台自身快速转动丢失目标问��    {
    lostTargetShortTime_toDo();

    if (lost >= lostmin && lost < lostmax) //丢失时间较短，处理猫步或陀螺切换装甲板导致目标短暂消失问题
    {
        lostTargetLongTime_toDo();
    }
    if (lost >= lostmax) //目标可能的确走了，即不在视野中，而不是因为没识别��
    {
        is_insight = false;
    }
}

void SentryPictureManipulator::writeIntoFilterDataCSV()
{
    //输出数据到csv文件，方便赛后检查数据是否异常，因为视频太快，其中某帧出问题的话，人可能看不出来有问题
    filterData << (double)clock() / CLOCKS_PER_SEC << ","
               << armor_data.yaw_angle << ","
               << armor_data.pitch_angle << ","
               << armor_data.atocDistance << endl;
}

int SentryPictureManipulator::judgeTargetInsight()
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
