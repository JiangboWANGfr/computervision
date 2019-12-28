/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 13:14:07 +0800
 * @LastEditTime : 2019-12-29 00:00:39
 * @LastEditors  : Please set LastEditors
 * @Description: 
 */


#include "BuffPictureManipulator.h"
#include "BuffDetector.h"

#ifdef BUFF

BuffPictureManipulator::BuffPictureManipulator() : PictureManipulator()
{
}

BuffPictureManipulator::~BuffPictureManipulator()
{
}

int BuffPictureManipulator::manipulatePicture(Mat source_image_directly_from_camera)
{
    detect_buff.getCenter(source_image_directly_from_camera, armor_data); 
}


#endif