/*
 * @Copyright: CS of BIT
 * @Author: 王江波
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:33:12 +0800
 * @LastEditTime: 2019-10-12 20:50:26
 * @LastEditors: 
 * @Description: 
 */
#include "HeroAngleCalculate.hpp"
#ifdef BIGARMOR //识别大装甲板
B_S = true; 

#endif
#ifdef Hero
void AngleCalculate::SetParameter_ForPnpSover(Armor &target)
{
    
    //装甲板在世界坐标系中的坐标
    centerpoint_in_realworld_axis.clear();
    if (B_S)
    {
        centerpoint_in_realworld_axis.push_back(cv::Point3f(0, 0, 0));
        centerpoint_in_realworld_axis.push_back(cv::Point3f(229, 0, 0));
        centerpoint_in_realworld_axis.push_back(cv::Point3f(299, 55, 0));
        centerpoint_in_realworld_axis.push_back(cv::Point3f(0, 55, 0));
    }
    else
    {
        centerpoint_in_realworld_axis.push_back(cv::Point3f(0, 0, 0));
        centerpoint_in_realworld_axis.push_back(cv::Point3f(134, 0, 0));
        centerpoint_in_realworld_axis.push_back(cv::Point3f(134, 55, 0));
        centerpoint_in_realworld_axis.push_back(cv::Point3f(0, 55, 0));
    }
    //装甲板在图像坐标系下的坐标，因为裁剪过所以centerpoint_in_camera_axis等于target.rect[0] + cv::Point2f(pre_pt)
    //target.rect[]表示在裁剪过的图像中的坐标，Point2f(pre_pt)表示原始图片中的左下角的点
    centerpoint_in_camera_axis.clear();
    centerpoint_in_camera_axis.push_back(target.rect[0]);
    centerpoint_in_camera_axis.push_back(target.rect[2]);
    centerpoint_in_camera_axis.push_back(target.rect[3]);
    centerpoint_in_camera_axis.push_back(target.rect[1]);
    //设置相机内参
    _A_matrix.at<double>(0, 0) = 1453.68113; //      [ fx   0  cx ]
    _A_matrix.at<double>(1, 1) = 1453.28168; //      [  0  fy  cy ]
    _A_matrix.at<double>(0, 2) = 640;        //      [  0   0   1 ]
    _A_matrix.at<double>(1, 2) = 512;
    _A_matrix.at<double>(2, 2) = 1;
    //设置相机的畸变系统
    distCoeffs.at<double>(0, 0) = -0.07301;
    distCoeffs.at<double>(0, 1) = 0.18455;
    distCoeffs.at<double>(0, 2) = 0.00017;
    distCoeffs.at<double>(0, 3) = -0.00115;
    distCoeffs.at<double>(0, 4) = 0.0;
}
//计算x y z
void AngleCalculate::Calculate_x_y_z_in_camera_axis(Angle &pnpresult)
{
    double m00, m01, m02;
    double m10, m11, m12;
    double m20, m21, m22;
    m00 = _R_matrix.at<double>(0, 0);
    m01 = _R_matrix.at<double>(0, 1);
    m02 = _R_matrix.at<double>(0, 2);
    m10 = _R_matrix.at<double>(1, 0);
    m11 = _R_matrix.at<double>(1, 1);
    m12 = _R_matrix.at<double>(1, 2);
    m20 = _R_matrix.at<double>(2, 0);
    m21 = _R_matrix.at<double>(2, 0);
    m22 = _R_matrix.at<double>(2, 2);
    char tam1[100];
    if (B_S)
    {
        newx = m00 * 114.5 + m01 * 27.5 + m02 * 0 + _t_matrix.at<double>(0, 0) - 76.4; //-76.4
        newy = m10 * 114.5 + m11 * 27.5 + m12 * 0 + _t_matrix.at<double>(1, 0) - 80;   //-80
        newz = m20 * 114.5 + m21 * 27.5 + m22 * 0 + _t_matrix.at<double>(2, 0);
        std::cout << "big armor!!!" << endl;
        std::sprintf(tam1, "B");
    }
    else
    {
        newx = m00 * 67 + m01 * 27.5 + m02 * 0 + _t_matrix.at<double>(0, 0) - 76.4; //76.4->35
        newy = m10 * 67 + m11 * 27.5 + m12 * 0 + _t_matrix.at<double>(1, 0) - 80;   //48.5->130
        newz = m20 * 67 + m21 * 27.5 + m22 * 0 + _t_matrix.at<double>(2, 0);
        std::cout << "small armor!!!" << endl;
        std::sprintf(tam1, "S");
    }
    char tam2[100];
    std::sprintf(tam2, "center in cam(%0.0f,%0.0f,%0.0f)", newx, newy, newz);
    if (std::abs(newx) < 6000)
        pnpresult.x = newx;
    if (std::abs(newy) < 6000)
        pnpresult.y = newy;
    if (std::abs(newz) < 12000)
        pnpresult.z = newz;
    pnpresult.find_armor = true;
}
//计算Pitch和yaw轴
void AngleCalculate::calculate_pitch_and_yaw(Angle &pnpresult)
{
    float pitch, yaw;
    double vec[3];
    vec[0] = newx;
    vec[1] = newy;
    vec[2] = newz;
    yaw = std::atan(vec[0] / vec[2]) * 180 / CV_PI;
    pitch = std::atan(vec[1] / vec[2]) * 180 / CV_PI; //因与刘的方向相反而作修改,zjh于2019.7.2
    pnpresult.yaw = yaw;
    pnpresult.pitch = pitch;
    pnpresult.distance = sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
    char tam3[100];
    //std::sprintf(tam3, "tan yaw=%0.4f   tan pich=%0.4f", vec[0] / vec[2], vec[1] / vec[2]);
    char tam4[100];
    //std::sprintf(tam4, "yaw=%0.4f   pitch=%0.4f", pnpresult.yaw, pnpresult.pitch);
}

void AngleCalculate::pnpSolver(Angle &pnpresult,Armor &target)
{
    SetParameter_ForPnpSover(target);
    cv::solvePnP(centerpoint_in_realworld_axis, centerpoint_in_camera_axis, _A_matrix, distCoeffs, rvec, tvec, false, SOLVEPNP_AP3P);
    cv::Rodrigues(rvec, _R_matrix); // converts Rotation Vector to Matrix
    _t_matrix = tvec;               // set translation matrix
    Calculate_x_y_z_in_camera_axis(pnpresult);
    calculate_pitch_and_yaw(pnpresult);
}

#endif