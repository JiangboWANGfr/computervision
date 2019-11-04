/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:36:33 +0800
 * @LastEditTime: 2019-10-12 20:50:16
 * @LastEditors: 
 * @Description: 
 */

#ifndef ANGLECALCULATE_HPP
#define ANGLECALCULATE_HPP
#include "header.h"
#include "HeroArmorDetector.hpp"
#ifdef Hero
class AngleCalculate
{
    public:
    void pnpSolver(Angle m_pnpresult,int hh,Armor m_target,cv::Point2f pre_pt);
    private:
    Angle pnpresult;
    Armor target;
    cv::Point2f pre_pt;
    std::vector<cv::Point3f> centerpoint_in_realworld_axis;
    std::vector<cv::Point2f> centerpoint_in_camera_axis;
    cv::Mat _A_matrix = cv::Mat::zeros(3, 3, CV_64FC1);  // intrinsic camera parameters
    cv::Mat distCoeffs = cv::Mat::zeros(4, 1, CV_64FC1); // vector of distortion coefficients
    cv::Mat _R_matrix = cv::Mat::zeros(3, 3, CV_64FC1); // rotation matrix
    cv::Mat _t_matrix = cv::Mat::zeros(3, 1, CV_64FC1); // translation matrix
    cv::Mat rvec = cv::Mat::zeros(3, 1, CV_64FC1);      // output rotation vector
    cv::Mat tvec = cv::Mat::zeros(3, 1, CV_64FC1);
    double newx = 0.0, newy = 0.0, newz = 0.0;
    int hh;
    void SetParameter_ForPnpSover(std::vector<cv::Point3f> &centerpoint_in_realworld_axis,std::vector<cv::Point2f> &centerpoint_in_camera_axis,Armor target, cv::Mat &_A_matrix, cv::Mat &distCoeffs);
    void Calculate_x_y_z_in_camera_axis(Angle m_pnpresult,int &hh, cv::Mat &_R_matrix,cv::Mat &_t_matrix,double &newx,double &newy,double &newzz);
    void calculate_pitch_and_yaw(Angle m_pnpresult,double &newx, double &newy, double &newz);
    
};

#endif
#endif