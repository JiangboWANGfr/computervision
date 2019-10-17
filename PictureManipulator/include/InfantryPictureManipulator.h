/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-10-07 18:20:39
 * @LastEditTime: 2019-10-12 21:00:40
 * @LastEditors: Please set LastEditors
 */
#ifndef __INFANTRY_PICTURE_MANIPULATOR_H__
#define __INFANTRY_PICTURE_MANIPULATOR_H__

#include "header.h"
#include "SivirDetector.h"
#include "PictureManipulator.h"

class InfantryPictureManipulator : public PictureManipulator
{
private:
    void DetermineRecognitionMode();
    void IdentifyAramorMode(Mat &frame);
    void IdentifyMode2(Mat &frame );
    void GetAramor();
    void IdentifyTarget();
    SivirDetector Sivir;

public:
    InfantryPictureManipulator(string serial_port_device,
                             string path,
                             double fps,
                             int width_video_size,
                             int height_video_size);
    InfantryPictureManipulator();
    ~InfantryPictureManipulator();
    int manipulatePicture(Mat picture);
};
#endif
