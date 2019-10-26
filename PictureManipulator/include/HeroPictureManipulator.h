/*
 * @Copyright: CS of BIT
 * @Author: 王江波
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 12:59:35 +0800
 * @LastEditTime: 2019-10-12 21:02:22
 * @LastEditors: 
 * @Description: 
 */
#ifndef _Hero_PICTURE_MANIPULATOR_H__
#define _Hero_PICTURE_MANIPULATOR_H__
#include "header.h"
#include "PictureManipulator.h"
#ifdef Hero

class HeroPictureManipulator : public PictureManipulator
{
private:
    void writeIntoFilterDataCSV();

public:
    HeroPictureManipulator(string serial_port_device,
                             string path,
                             double fps = 120,
                             int width_video_size = 640,
                             int height_video_size = 480);
    HeroPictureManipulator();
    ~HeroPictureManipulator();
    int manipulatePicture(Mat source_image_directly_from_camera);
};

#endif
#endif