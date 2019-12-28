/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 12:59:35 +0800
 * @LastEditTime : 2019-12-28 22:42:44
 * @LastEditors  : Please set LastEditors
 * @Description: 
 */


#ifndef __BUFF_PICTURE_MANIPULATOR_H__
#define __BUFF_PICTURE_MANIPULATOR_H__
#include "header.h"
#include "PictureManipulator.h"
#include "BuffDetector.h"

#ifdef BUFF

class BuffPictureManipulator : public PictureManipulator
{
private:
    bool is_insight;
    int lost;
    BuffDetectorWrapper detect_buff; 

public:
    BuffPictureManipulator();
    ~BuffPictureManipulator();
    int manipulatePicture(Mat picture);
};

#endif

#endif