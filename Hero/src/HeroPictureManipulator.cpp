/*
 * @Descripttion: 
 * @version: 
 * @Author: zzdr
 * @Date: 2019-10-08 12:02:46
 * @LastEditors: zzdr
 * @LastEditTime: 2019-10-22 07:30:46
 */
#include "Controller.h"
#include "HeroPictureManipulator.h"

#ifdef Hero
HeroPictureManipulator::HeroPictureManipulator() : PictureManipulator()
{
}


HeroPictureManipulator::~HeroPictureManipulator()
{
}

int HeroPictureManipulator::initArmorData(TargetData &armor_data)
{
    armor_data.yaw_angle = 0;
    armor_data.pitch_angle = 0;
    armor_data.atocDistance = 0;
    armor_data.is_big = false;
    armor_data.is_get = false;
}

int HeroPictureManipulator::manipulatePicture(Mat source_image_directly_from_camera)
{

    int loop_num;
    //armor_detector.setup();
    armor_detector.R_B = true; // Blue armor!
    double start = clock();
    cv::Mat g_srcImage = source_image_directly_from_camera;
    //src_video << g_srcImage; //保存图片
    cout << "\n\n--------------------第\t" << loop_num++ << "帧--------------------" << endl;
    // armor_detector.cut(g_srcImage);
    // armor_detector.Bright();
    // armor_detector.BrighttoCanny();
    // armor_detector.filter();
    armor_detector.getCenter(g_srcImage,armor_data);
    

    cout << "------------------------------------------------\n"<< endl;
    //fin_video << g_srcImage; //保存处理后的图片


#ifdef STM32
    //串口通信发送信息给电控stm32,英雄
    //stm32.sendAngle(armor_data.yaw_angle, armor_data.pitch_angle, armor_data.atocDistance,
            //        armor_data.is_big, is_insight, armor_data.is_get);
#endif

    writeIntoFilterDataCSV();

#ifdef DEBUG1
    showPicture("armor", img, 2);
    imshow("armor", img);
#endif

    double end = clock();
    std::cout << "Manipulate Time Per Pic:" << (end - start) / CLOCKS_PER_SEC * 1000 << "ms\n"
              << endl;
}

void HeroPictureManipulator::writeIntoFilterDataCSV()
{
    //输出数据到csv文件，方便赛后检查数据是否异常，因为视频太快，其中某帧出问题的话，人可能看不出来有问题
    // filterData << (double)clock() / CLOCKS_PER_SEC << ","
    //            << armor_data.yaw_angle << ","
    //            << armor_data.pitch_angle << ","
    //            << armor_data.atocDistance << endl;
}

#endif

