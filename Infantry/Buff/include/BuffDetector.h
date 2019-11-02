/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:26:02 +0800
 * @LastEditTime: 2019-11-02 11:21:39
 * @LastEditors: 
 * @Description: 
 */

#ifndef __BUFF_DETECTOR_H__
#define __BUFF_DETECTOR_H__

#define CAMERA1_FILEPATH "/home/waltpeter/BuffDetector_ZHBIT/param/camera_param/\
galaxy_0.xml"

#include "header.h"
#include "SolveAngle.h"

#define BUFF_DETECT_DEBUG
#ifdef BUFF_DETECT_DEBUG
// ---- buff debug ----
#define DEBUG_DRAW_CONTOURS
#define DEBUG_PUT_TEST_TARGET
#define DEBUG_PUT_TEST_ANGLE
#define DEBUG_DRAW_TARGET
//#define TEST_OTSU
#define AREA_LENGTH_ANGLE 1 // 1:area 2:length 3:diff_angle
#define FUSION_MINAREA_ELLIPASE
#define DIRECTION_FILTER
#define IMSHOW_2_ROI
// ---- buff debug ----
#endif

// 固定状态
#define BULLET_SPEED 28.5
#define BUFF_H 800
#define BUFF_DISTANCE 7300

struct OtherParam
{
    int color = 1;       // 我方车辆颜色，0是蓝色，1是红色。用于图像预处理
    int mode = 0;        // 视觉模式，0是自瞄模式，1是能量机关模式
    int cap_mode = 1;    // 摄像头类型，0是短焦摄像头，1是长焦摄像头
    float gimbal_data;
    float buff_offset_x;
    float buff_offset_y;
};

// 摄像头坐标系到云台坐标系
#define SHOR_X 57.0f
#define SHOR_Y 47.5f
#define SHOR_Z -111.37f
#define LONG_X 0.0f
#define LONG_Y 40.7f
#define LONG_Z -123.0f
#define PTZ_TO_BARREL 0.0f   // 补兵激光在２３ｍｍ下方

// 调试状态
// 1:    2:   3:
#define BUFF_OFFSET_x 101// 1:80// 3:112
#define BUFF_OFFSET_y 118// 1:125// 3:69

#define WORLD_OFFSET_X 750
#define COLOR_TH 20

#define DEFAULT 0
#define FIRE 3
#define RESET 4

typedef enum{UNKOWN,INACTION,ACTION}ObjectType;
typedef enum{restore_center=6,follow=-1,shoot=3}mode_buff;

class BuffDetectorWrapper
{
private:
    float distance = 0.0; 
    float yaw = 0.0; 
    float pitch = 0.0; 
    int command = 0;
    OtherParam other_param;
public:
    BuffDetectorWrapper(){}; 
    ~BuffDetectorWrapper(){};
    void getCenter(cv::Mat &source_img, TargetData &armor_data);
    void initializeParam(); 
    float getYaw(); 
    float getPitch(); 
    float getDistance(); 
private:
    
};

/**
 * @brief 矩形类物体属性
 * 在逻辑识别部分需要修改原有旋转矩形属性
 * 在计算0-360角度上需要用到旋转矩形原始参数
 */
class Object
{
public:
    Object(){}
    void DrawTarget(Mat &img)
    {
        if(type_ == INACTION)
            circle(img, small_rect_.center, 3, Scalar(0, 0, 255), -1);
        else if(type_ == ACTION)
            circle(img, small_rect_.center, 3, Scalar(255, 255, 255), -1);
        else
            circle(img, small_rect_.center, 3, Scalar(255, 255, 255), 1);
    }

    void UpdateOrder(); // 更新能量机关装甲板的绝对位置
    void KnowYourself(Mat &img);    //判断能量机关扇叶的状态（激活　未激活）

    RotatedRect small_rect_;    // 能量机关扇叶内轮廓
    RotatedRect big_rect_;  // 能量机关扇叶外轮廓
    vector<Point2f> points_2d_; // ｐｎｐ角度解算的四个点
    float angle_;
    float diff_angle;
    int type_ = UNKOWN;
};

/**
 * @brief 能量机关复位任务
 */
class ResetTask{
public:
    ResetTask(){}
    // 复位归中条件是，丢失目标超过几帧
    int run(bool find_flag)
    {
        int command = DEFAULT;
        if(find_flag == false){
            cnt++;
        }else{
            cnt = 0;
        }
        // 判断归中条件
        if(cnt > max_cnt_)
        {
            command = RESET;

        }else{
            command = DEFAULT;
        }
        return command;
    }
public:
    int cnt = 0;
    int max_cnt_ = 30;   // 最大丢失目标次数
};

double calcDistanceFor2Point(Point2f p1, Point2f p2);

/**
 * @brief BuffDetectTask 能量机关识别总任务，每帧调用
 * @param img 摄像头获取的RGB图像
 * @return 1是发现目标，0是未发现目标
 */
class BuffDetector
{
public:
    BuffDetector(){
        solve_angle_long_ = SolveAngle(CAMERA1_FILEPATH, LONG_X, LONG_Y, LONG_Z, PTZ_TO_BARREL);
        readXML();  // 读取调整参数
    }
    ~BuffDetector(){}

    int BuffDetectTask(Mat& img, OtherParam param);
    void getAngle(float &yaw, float &pitch){
        yaw = angle_x_;
        pitch = angle_y_;
    }

    /**
     * @brief 辨别能量机关旋转方向
     * 根据每次识别得到的角度进行滤波，判断能量机关旋转方向
     */
    float getDistance(){
        return distance_;
    }

    /**
     * @brief 判断方向
     * @param 输入能量机关角度
     * @return
     */
    int getSimpleDirection(float angle);

    void readXML(){
        FileStorage fs("../param/buff_offset.xml", FileStorage::READ);
        fs["offset_x"] >> buff_offset_x_;
        fs["offset_y"] >> buff_offset_y_;

        if(buff_offset_x_ <= 0 || buff_offset_x_ >= 200)
            buff_offset_x_ = BUFF_OFFSET_x;
        if(buff_offset_y_ <= 0 || buff_offset_y_ >= 200)
            buff_offset_y_ = BUFF_OFFSET_y;
        begin_offset_x_ = buff_offset_x_;
        begin_offset_y_ = buff_offset_y_;
        cout << "read:"<<buff_offset_x_ << ", " << buff_offset_y_ << endl;
        fs.release();
    }
    void writeXML(){
        FileStorage fs("../param/buff_offset.xml", FileStorage::WRITE);
        fs << "offset_x" << buff_offset_x_;
        fs << "offset_y" << buff_offset_y_;

        cout << "write:"<<buff_offset_x_ << ", " << buff_offset_y_ << endl;
        fs.release();
    }
private:
    bool DetectBuff(Mat& img, OtherParam other_param);

    // 外部参数
private:
    int color_;
    float gimbal;

    // debug参数
public:
    int buff_offset_x_ = BUFF_OFFSET_x;//id:3 112;// id:2 130;
    int buff_offset_y_ = BUFF_OFFSET_y;//id:3 69;// id:2 135;
    int begin_offset_x_ = BUFF_OFFSET_x;
    int begin_offset_y_ = BUFF_OFFSET_y;


    int world_offset_x_ = WORLD_OFFSET_X;
    int world_offset_y_ = 500;
    int pitch_offset = 2000;
    int color_th_ = COLOR_TH;
    int gray_th_ = 50;
    float buff_angle_ = 0;
    float diff_angle_ = 0;
    int area_ratio_ = 500;

    //相关类申明
    //AutoControl auto_control;
private:
    SolveAngle solve_angle_long_;
    //MainWindow *w_;

private:
    float angle_x_ = 0;
    float angle_y_ = 0;
    float last_angle_x_ = 0;
    float last_angle_y_ = 0;
    float distance_ = 0;
    vector<Point2f> points_2d;
    int action_cnt_ = 0;
    vector<float> vec_diff_angle;

private:
    float d_angle_ = 0;
    float r = 0.1;
    int last_angle_;
    int find_cnt = 0;
    int direction_tmp=0;

public:
    int waitkey_flag = 1;
    int imshow_flag = 1;


    int command = 0;
};

double Point_distance(Point2f p1,Point2f p2);

#endif
