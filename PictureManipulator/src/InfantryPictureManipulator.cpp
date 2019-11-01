/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 13:14:07 +0800
 * @LastEditTime: 2019-11-01 19:42:44
 * @LastEditors: 
 * @Description: 
 */


#include "InfantryPictureManipulator.h"
#include "ArmorDetector.h"

#ifdef INFANTRY


InfantryPictureManipulator::InfantryPictureManipulator(string serial_port_device,
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

InfantryPictureManipulator::InfantryPictureManipulator()
{
}

InfantryPictureManipulator::~InfantryPictureManipulator()
{
}

int InfantryPictureManipulator::initArmorData(TargetData &armor_data)
{
    armor_data.yaw_angle = 0;
    armor_data.pitch_angle = 0;
    armor_data.atocDistance = 0;
    armor_data.is_big = false;
    armor_data.is_get = false;
}

int InfantryPictureManipulator::verifyMode(int &mode, int &isred)
{
    switch(mode)
    {
        case 1:         //识别小装甲板 
        case 2:         //识别大装甲板
        case 3: findmode = ARMOR_MODE;
            break;      //大小装甲板均识别
            
        case 4:         //静止能量机关
        case 5: findmode = SIVIR_MODE;
            break;      //转动
            
        // case 7: isred = true;
        //     break;      //敌方是红色
        // case 8: isred = false;
        //     break;

        default: break;
    }
    cout<<"mode: "<<mode<<endl;
    cout<<"findmode: "<<this->findmode<<endl;
}

void InfantryPictureManipulator::armorModeToDo(Mat srcimg)
{
    armor_detector.getCenter(srcimg, armor_data);
}

void InfantryPictureManipulator::sivirModeToDo()
{
}

int InfantryPictureManipulator::manipulatePicture(Mat srcimg)
{
    initArmorData(armor_data);
    verifyMode(armor_detector.mode, armor_detector.isred);
    if(this->findmode == ARMOR_MODE)
    {
        armorModeToDo(srcimg);
    }
        
    else 
        sivirModeToDo();
}

#endif