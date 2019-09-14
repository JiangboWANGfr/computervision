/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:33:12 +0800
 * @LastEditTime: 2019-08-31 10:33:12 +0800
 * @LastEditors: 
 * @Description: 
 */
#include "AngleCalculate.hpp"

Mat_<float> intrinsic_matrix_8_640 = (Mat_<float>(3, 3) << 1453.68113, 0, 329.79786-8,
                                                        0, 1453.28168, 251.06453-6,
                                                        0, 0, 1);   //cx,cy项-8.-6是因为获取图片时的截图导致原点偏移
vector<float> distCoeff_8_640 = { -0.07301, 0.18455, -0.00017, -0.00115, 0 };

void AngleCalculate::pnpSolver(cv::RotatedRect &R1, cv::RotatedRect &R2, TargetData &point4data)
{
    // 准备pnp所需四点的世界坐标系3D坐标,小装甲灯柱对中心宽134mm，大装甲灯柱对中心宽229mm，高55mm
    // 世界坐标系中心建立在装甲板中心
    vector<cv::Point3f> Points3D;
    if(point4data.is_big == 0)
    {
        Points3D.push_back(cv::Point3f(-67, -27.5, 0));        //P1 三维坐标的单位是毫米
        Points3D.push_back(cv::Point3f(-67, 27.5, 0));         //P2
        Points3D.push_back(cv::Point3f(67, -27.5, 0));         //P3
        Points3D.push_back(cv::Point3f(67, 27.5, 0));          //P4
    }
    else
    {
        Points3D.push_back(cv::Point3f(-114.5, -27.5, 0));        //P1 三维坐标的单位是毫米
        Points3D.push_back(cv::Point3f(-114.5, 27.5, 0));         //P2
        Points3D.push_back(cv::Point3f(114.5, -27.5, 0));         //P3
        Points3D.push_back(cv::Point3f(114.5, 27.5, 0));          //P4
    }
    
    // 准备pnp所需四点在2D图中对应的坐标
    vector<cv::Point2f> Points2D;
    Points2D.push_back(cv::Point2f(R1.center.x + 0.5*R1.size.height*sin(R1.angle/180*CV_PI), R1.center.y-0.5*R1.size.height*cos(R1.angle/180*CV_PI)));        //P1 单位是像素
    Points2D.push_back(cv::Point2f(R1.center.x - 0.5*R1.size.height*sin(R1.angle/180*CV_PI), R1.center.y+0.5*R1.size.height*cos(R1.angle/180*CV_PI)));        //P2
    Points2D.push_back(cv::Point2f(R2.center.x + 0.5*R2.size.height*sin(R2.angle/180*CV_PI), R2.center.y-0.5*R2.size.height*cos(R2.angle/180*CV_PI)));        //P3
    Points2D.push_back(cv::Point2f(R2.center.x - 0.5*R2.size.height*sin(R2.angle/180*CV_PI), R2.center.y+0.5*R2.size.height*cos(R2.angle/180*CV_PI)));        //P4

    // 初始化输出矩阵
    cv::Mat rvec = cv::Mat::zeros(3, 1, CV_64FC1);
    cv::Mat tvec = cv::Mat::zeros(3, 1, CV_64FC1);
    
    float RatioX = 1.0f;
    float RatioY = 1.0f;
    float RatioZ = 1.0f;
    float xFix = 0.0f;
    float yFix = 0.0f;

    solvePnP(Points3D, Points2D, intrinsic_matrix_8_640, distCoeff_8_640, rvec, tvec, false, SOLVEPNP_AP3P);
    /*
        solvePnP可选算法有好几个，csdn常见用p3p，本人经过实践p3p错误率有点高，AP3P要好一点
        p3p时03年提出的，AP3P是17年的，可以看出CSDN的滞后性，随着opencv版本更新，许多算法都会得到优化，所以不必拘泥于旧版本
        pnp计算问题需要二维点和三维点对应，然后二维点是图像处理后的量，与实际的点有出入，下方的弹道修正代码是根据距离修的，距离不准影响修正
        解决由于图像处理引起的误差的方式常见的有乘以某个系数，或者想办法让处理后的图像上的点与实际点对应
        另外由于曝光时间内物体的相对运动导致成像时灯条被拉变形，此时取哪些点更能反映真实情况是值得考虑的事情
    */

    // 由Pc = RPw + T解算物体在相机坐标系的坐标,(0,37,132)为相机坐标系到云台中心坐标系的平移，具体数据问机械画图的人要
    point4data.x = RatioX * tvec.ptr<double>(0)[0];
    point4data.y = RatioY * (tvec.ptr<double>(0)[1] + 37 );
    point4data.z = RatioZ * (tvec.ptr<double>(0)[2] + 132 );
    point4data.atocDistance = sqrt(point4data.x*point4data.x + point4data.y*point4data.y + point4data.z*point4data.z);       
    // 计算横向偏移做好修正,弹道偏右想往左挪修正量为负，反之为正(图片坐标系下)
    // 计算纵向下坠量做好补偿，弹道偏下想往上挪修正量为负，反之为正(图片坐标系下)        
    // 偏移量和下坠量以实际机器打弹测试为准，不要相信红点，红点会装歪
    if(point4data.atocDistance  > 2000)
    {
        xFix = (point4data.atocDistance-2000)/1000*(-15);
        point4data.x = point4data.x + xFix; 
        yFix = (point4data.atocDistance-2000)/1000*60;
        point4data.y = point4data.y - yFix;
    }
    // 远距离时弹道下坠严重，需要另修，复活赛时哨兵最远识别到地方敌方的工程，修弹道后可以打中敌方桥头的步兵，但此时云台有仰角，此弹丸初速度有向上的分量
    if(point4data.atocDistance > 6200)
    {

    }
    if(point4data.atocDistance > 7500)
    {

    }
    // 计算两轴偏角，yaw向右为正，pitch向左为正
    point4data.yaw_angle = atan(point4data.x/point4data.z)*180/CV_PI;
    point4data.pitch_angle = atan(point4data.y/point4data.z)*180/CV_PI;       
    // 角度太小赋0.0f防止抖动
    point4data.yaw_angle = abs(point4data.yaw_angle) > 0.01f ? point4data.yaw_angle : 0.0f;
    point4data.pitch_angle = abs(point4data.pitch_angle) > 0.01f ? point4data.pitch_angle : 0.0f;
    // 划定打击区域,静止时1.5°已经到达边沿，放的越大越暴力，后来电控做了预测后根据他们自己的量判断是否打击
    if( (abs(point4data.yaw_angle) < 1.5) && (abs(point4data.pitch_angle) < 1.5) )
    {
        point4data.shoot = true;
    }
    else
    {
        point4data.shoot = false;
    }
    // 距离单位转换为m
    point4data.atocDistance = point4data.atocDistance/1000;
}
