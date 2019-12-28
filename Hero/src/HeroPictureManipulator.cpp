/*
 * @Descripttion: 
 * @version: 
 * @Author: zzdr
 * @Date: 2019-10-08 12:02:46
 * @LastEditors  : zzdr
 * @LastEditTime : 2019-12-28 17:05:48
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
    initArmorData(armor_data);
    //armor_detector.setup();
    armor_detector.R_B = true; // Blue armor!
    //double start = clock();
    //cv::Mat g_srcImage = source_image_directly_from_camera;
    //src_video << g_srcImage; //保存图片
    //cout << "\n\n--------------------第\t" << loop_num++ << "帧--------------------" << endl;
    // armor_detector.cut(g_srcImage);
    // armor_detector.Bright();
    // armor_detector.BrighttoCanny();
    // armor_detector.filter();
    armor_detector.getCenter(source_image_directly_from_camera,armor_data);
    //cout << "------------------------------------------------\n"<< endl;
    //fin_video << g_srcImage; //保存处理后的图片
   // double end = clock();

}


#endif

