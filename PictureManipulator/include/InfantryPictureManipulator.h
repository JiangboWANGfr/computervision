/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-10-07 18:20:39
 * @LastEditTime: 2019-10-18 10:09:36 +0800
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
    InfantryPictureManipulator();
    ~InfantryPictureManipulator();
    int manipulatePicture(Mat picture);
};



#endif


#endif