/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-10-07 18:20:39
 * @LastEditTime: 2019-10-17 21:50:27 +0800
 * @LastEditors: Please set LastEditors
 */


#ifndef __INFANTRY_PICTURE_MANIPULATOR_H__
#define __INFANTRY_PICTURE_MANIPULATOR_H__

#include "header.h"
#include "SivirDetector.h"
#include "PictureManipulator.h"

#ifdef INFANTRY

class InfantryPictureManipulator : public PictureManipulator
{
private:
    bool is_insight;
    int lost;
    
private:
    void initArmorData();

    void lostTargetShortTime_toDo();

    void lostTargetLongTime_toDo();

    void maybeLostTarget();

    void writeIntoFilterDataCSV();

    int judgeTargetInsight();

public:
    InfantryPictureManipulator(string serial_port_device,
                             string path,
                             double fps = 120,
                             int width_video_size = 640,
                             int height_video_size = 480);
    InfantryPictureManipulator();
    ~InfantryPictureManipulator();
    int manipulatePicture(Mat picture);
};



#endif


#endif