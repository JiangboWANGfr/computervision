/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 12:59:35 +0800
 * @LastEditTime: 2019-10-17 23:12:33 +0800
 * @LastEditors: 
 * @Description: 
 */


#ifndef __SENTRY_PICTURE_MANIPULATOR_H__
#define __SENTRY_PICTURE_MANIPULATOR_H__
#include "header.h"
#include "PictureManipulator.h"

#ifdef SENTRY

class SentryPictureManipulator : public PictureManipulator
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
    SentryPictureManipulator();
    ~SentryPictureManipulator();
    int manipulatePicture(Mat picture);
};

#endif

#endif