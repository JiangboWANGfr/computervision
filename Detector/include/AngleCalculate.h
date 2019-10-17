/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:36:33 +0800
 * @LastEditTime: 2019-10-05 03:43:51 -0700
 * @LastEditors: 
 * @Description: 
 */
#ifndef __ANGLE_CALCULATE_H__
#define __ANGLE_CALCULATE_H__

#include "header.h"

class AngleCalculate
{
    public:
    void pnpSolver(cv::RotatedRect &R1, cv::RotatedRect &R2, TargetData &point4data);
    
    private:
    
};

#endif
