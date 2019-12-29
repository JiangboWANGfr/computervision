/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-10-12 17:23:32
 * @LastEditTime : 2019-12-29 16:10:11
 * @LastEditors  : Please set LastEditors
 */
#pragma once
#include "header.h"

#ifdef BUFF

// 一阶ｋａｌｍａｎ滤波
class Kalman1
{
public:
    Kalman1(){
        Q_ = 0.01f;
        R_ = 0.02f;
        t_ = 1.0f;
        x_ = 0.0f;
        p_ = 0.01f;
    }
    Kalman1(float Q, float R, float t, float x0, float p0){
        Q_ = Q;
        R_ = R;
        t_ = t;
        x_ = x0;
        p_ = p0;
    }
    void setParam(int R, int Q, int t){
        if(R<1)
            R=1;
        if(Q<1)
            Q=1;
        if(t<1)
            t=1;
        R_ = static_cast<float>(R*0.01f);
        Q_ = static_cast<float>(Q*0.01f);
        t_ = static_cast<float>(t);
    }
    float run(float data){
       x_pre_ = x_;                                      //x(k|k-1) = AX(k-1|k-1)+BU(k)
        p_pre_ = p_ + Q_;                              //p(k|k-1) = Ap(k-1|k-1)A'+Q
        kg_ = p_pre_ / (p_pre_ + R_);               //kg(k) = p(k|k-1)H'/(Hp(k|k-1)'+R)
        x_ = x_pre_ + kg_ * (data - x_pre_);          //x(k|k) = X(k|k-1)+kg(k)(Z(k)-HX(k|k-1))
        p_ = (1 - kg_) * p_pre_;                   //p(k|k) = (I-kg(k)H)P(k|k-1)
        return x_;
    }
    float merge_run(float data1, float data2)
    {
        x_pre_ = data1;
        p_pre_ = p_ + Q_;                              //p(k|k-1) = Ap(k-1|k-1)A'+Q
        kg_ = p_pre_ / (p_pre_ + R_);               //kg(k) = p(k|k-1)H'/(Hp(k|k-1)'+R)
        x_ = x_pre_ + kg_ * (data2 - x_pre_);          //x(k|k) = X(k|k-1)+kg(k)(Z(k)-HX(k|k-1))
        p_ = (1 - kg_) * p_pre_;                   //p(k|k) = (I-kg(k)H)P(k|k-1)
        return x_;
    }
public:
    float R_;
    float Q_;
    float p_pre_;
    float x_pre_;
    float x_;
    float p_;
    float kg_;
    float t_;

};

class SolveAngle
{
public:
    SolveAngle(){
        //std::cout << "cameraMatrix: " << cameraMatrix << "\ndistCoeffs: "; 
        //std::cout << distCoeffs << std::endl; 
    }
    SolveAngle(const char* file_path, float c_x, float c_y, float c_z, float barrel_y);
    // 普通角度解算
    void getAngle(vector<Point2f>& image_point, float ballet_speed, float& angle_x, float& angle_y, float &dist);
    // 能量机关角度解算

    void getBuffAngle(bool flag, vector<Point2f>& image_point, float ballet_speed, float buff_angle, float pre_angle, float gimbal_pitch, float &angle_x, float &angle_y, float &dist);
    float getBuffPitch(float dist, float tvec_y, float ballet_speed);

    // ---------ICRA--------------------
    void getAngle_ICRA(vector<Point2f>& image_point, float ballet_speed, float& angle_x, float& angle_y, float &dist);
    float GetPitch_ICRA(float x, float y, float v);
    float BulletModel_ICRA(float x, float v, float angle);
    // ---------/ICRA-------------------
    void Generate3DPoints(uint mode, Point2f offset_point);
    //Mat cameraMatrix, distCoeffs;
    
    














    Mat_<float> cameraMatrix = (Mat_<float>(3, 3) << 1453.68113, 0, 329.79786-8,
                                                     0, 1453.28168, 251.06453-6,
                                                     0, 0, 1);   //cx,cy项-8.-6是因为获取图片时的截图导致原点偏移
    //相机畸变参数
    vector<float> distCoeffs = {-0.07301, 0.18455, -0.00017, -0.00115, 0};
    
    










    
    Mat object_point_mat;
    vector<Point3f> objectPoints;
    vector<Point2f> projectedPoints;
    vector<Point2f> imagePoints;
    Mat rvec;
    Mat tvec;
    float height_world = 60.0;
    float overlap_dist = 100000.0;
        float barrel_ptz_offset_x = -0;
    float barrel_ptz_offset_y = -0; // mm   + ptz is up barrel

    float ptz_camera_x = 0;       // +left
    float ptz_camera_y = 52.5;       // + camera is  ptz
    float ptz_camera_z = -135;//-225;     // - camera is front ptz
    float scale = 0.99f;              // is calc distance scale not use pnp ,test

    Kalman1 kalman;
    int f_ = 1500;

public:
    float buff_h;

};

#include "BuffDetector.h"

class SimpleSolveAngle{
public:
    SimpleSolveAngle(){}
    SimpleSolveAngle(float fx, float fy, float cx, float cy, float f){
        fx_ = fx;
        fy_ = fy;
        cx_ = cx;
        cy_ = cy;
        f_ = f;
    }
    void getAngle(float x_screen, float y_screen, float dh, float &angle_x, float &angle_y, float& distance)
    {
        // 相机坐标系转换
        float camera_xyz[3];
        camera_xyz[2] = height_world * f_ / dh; // Z
        camera_xyz[0] = (x_screen - cx_) * camera_xyz[2] / fx_; // X
        camera_xyz[1] = (y_screen - cy_) * camera_xyz[2] / fy_; // Y
        // 相机坐标系转云台坐标系(不考虑旋转)
        float ptz_xyz[3];
        ptz_xyz[0] = camera_xyz[0] + ptz_camera_x; // X
        ptz_xyz[1] = camera_xyz[1] + ptz_camera_y; // Y
        ptz_xyz[2] = camera_xyz[2] + ptz_camera_z; // Z

        angle_x = atan(ptz_xyz[0] / ptz_xyz[2]);
        angle_x = static_cast<float>(angle_x) * 57.2957805f;
        angle_y = atan(ptz_xyz[1] / ptz_xyz[2]);
        angle_y = static_cast<float>(angle_y) * 57.2957805f;
        distance = ptz_xyz[2];
    }
public:
    float f_ = 1500;

private:
    float fx_;
    float fy_;
    float cx_;
    float cy_;
    float height_world = 60.0;
    float ptz_camera_x = 0;       // +left
    float ptz_camera_y = 52.5;       // + camera is  ptz
    float ptz_camera_z = -135;//-225;     // - camera is front ptz
};

void CodeRotateByZ(double x, double y, double thetaz, double& outx, double& outy);
void CodeRotateByY(double x, double z, double thetay, double& outx, double& outz);
void CodeRotateByX(double y, double z, double thetax, double& outy, double& outz);

#endif