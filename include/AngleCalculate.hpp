/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:36:33 +0800
 * @LastEditTime: 2019-08-31 10:36:33 +0800
 * @LastEditors: 
 * @Description: 
 */
#ifndef ANGLECALCULATE_HPP
#define ANGLECALCULATE_HPP

#include "Header.h"

class AngleCalculate
{
    public:
    void pnpSolver(cv::RotatedRect &R1, cv::RotatedRect &R2, ArmorData &point4data);
    
    private:
    
};

#endif