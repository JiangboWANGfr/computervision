/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 13:14:07 +0800
 * @LastEditTime: 2019-12-26 22:39:56
 * @LastEditors: 
 * @Description: 
 */


#include "InfantryPictureManipulator.h"
#include "ArmorDetector.h"

#ifdef INFANTRY


InfantryPictureManipulator::InfantryPictureManipulator() : PictureManipulator()
{
    cout << "InfantryPIctureManipulator::InfantryPictureManipulator success" << endl;
}

InfantryPictureManipulator::~InfantryPictureManipulator()
{
}

int InfantryPictureManipulator::initArmorData()
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

void InfantryPictureManipulator::printTargetData()
{
    cout << "armor_data.yaw_angle" << armor_data.yaw_angle << endl;
    cout << "armor_data.pitch_angle" << armor_data.pitch_angle << endl;
    cout << "armor_data.atocDistance" << armor_data.atocDistance << endl;
    cout << "armor_data.is_big" << armor_data.is_big << endl;
    cout << "armor_data.is_get" << armor_data.is_get << endl;
}

void InfantryPictureManipulator::armorModeToDo(Mat srcimg)
{
    armor_detector.getCenter(srcimg, armor_data);
    printTargetData();
}

void InfantryPictureManipulator::sivirModeToDo()
{
}

int InfantryPictureManipulator::manipulatePicture(Mat srcimg)
{
    initArmorData();
    verifyMode(armor_detector.mode, armor_detector.isred);
    if(this->findmode == ARMOR_MODE)
    {
        armorModeToDo(srcimg);
    }
        
    else 
        sivirModeToDo();
}

#endif