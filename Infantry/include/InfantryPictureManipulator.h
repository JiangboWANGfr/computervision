/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-10-07 18:20:39
 * @LastEditTime: 2019-10-26 20:03:31
 * @LastEditors: Please set LastEditors
 */


#ifndef __INFANTRY_PICTURE_MANIPULATOR_H__
#define __INFANTRY_PICTURE_MANIPULATOR_H__

#include "header.h"
#include "PictureManipulator.h"

#ifdef INFANTRY

class InfantryPictureManipulator : public PictureManipulator
{
private:
    int findmode = 0;
    enum Rec_Mode{      //识别模式赋值
        ARMOR_MODE = 1,
        SIVIR_MODE = 2
    };
    
private:
    int initArmorData(TargetData &armor_data);
    int verifyMode(int &mode, int &isred);
    void armorModeToDo(Mat srcimg);
    void sivirModeToDo();   //暂时置空，具体算法后续补充


public:
    InfantryPictureManipulator();
    ~InfantryPictureManipulator();
    int manipulatePicture(Mat srcimg);
};










#endif
#endif