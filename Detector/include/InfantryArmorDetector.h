/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-10-07 20:29:23
 * @LastEditTime: 2019-10-17 21:56:25 +0800
 * @LastEditors: Please set LastEditors
 */
#ifndef _INFANTRY_ARMOR_DETECTOR_H_
#define _INFANTRY_ARMOR_DETECTOR_H_

#include "header.h"
#include "ArmorDetector.h"

#ifdef INFANTRY
typedef struct ARMOR
{
    Point2f center;
    Point2f rect[4];
}Armor;

typedef struct ROI
{
    Point2f lefttop;
    int rwidth;
    int rheight;
    int level;
}Roi;

typedef struct ANGLE
{
    float yaw;
    float pitch;
    float dist;
}Angle;

/**
 * @Author: 孟文珩
 * @Description: 记录3种识别模式的不同参数，部分相同参数未计入
 * @Throw: 
 */
typedef struct MODE_PARAM
{
    float relative_position_radio;
    int relative_angle[4][2];
    float relative_area[5];
    float center_distance[2][2];
}ModeParam;

class InfantryArmorDetector : public ArmorDetector
{
public:
    Mat src;//source image
    Mat binary;//binary image
    Mat outline;//outline image
    bool islost;//1代表丢失
    bool isred;//识别红色或蓝色
    Armor target;
    Roi roi;
    Mat roiimg;
    Angle pnpresult;
    vector< vector<Point> > contours;
    int num;
    float matchrank[1500][1500];
    float dis[1500][1500];
    bool isbig;
    int mode;

private:
    int local_num_i,local_num_j;
    double longi,shorti;
    double longj,shortj;
    double anglei,anglej;
    double areai,areaj;
    double d;
    ModeParam mode_1 = {0.5, { {5,85},{8,82},{15,75},{18,72} }, {10,3,2,1.75,1.5}, { {7,1.5},{2.7,1.5} }};
    ModeParam mode_2 = {1.5, { {5,85},{7,83},{13,77},{20,70} }, {10,4,3,1.75,1.5}, { {7,1.5},{5,3} }};
    ModeParam mode_3 = {1.5, { {5,85},{7,83},{13,77},{20,70} }, {10,4,3,1.75,1.5}, { {4.7,1.5},{3,3} }};
    
private:
    double getLongSide(double side_1,double side_2);
    double getShortSide(double side_1,double side_2);
    void inclinationAngleFilter(vector<RotatedRect>& box);
    void aspectRadioFilter(vector<RotatedRect>& box);
    void relativePositionFilter(vector<RotatedRect>& box,ModeParam mode);
    void relativeTiltAngleFilter(ModeParam mode);
    void areaRadioFilter(ModeParam mode);
    void centerConnectionLengthFileterofMode_3(int i,int j);
    void centerConnectionLengthFileter(ModeParam mode);

private:
    int besti = -1,bestj = -1;
    int maxpoint=-8000;
    RotatedRect boxi;
    RotatedRect boxj;
    

private:
    void pickBest();
    void lostArmor();
public: 
    InfantryArmorDetector();
    InfantryArmorDetector(Mat src0);
    void getCenter(cv::Mat &source_img, TargetData &armor_data);
    void getSrcImage(Mat src0);
    void getBinaryImage();
    void getContours();
    void getTarget();
    void getPnP();

};





#endif
#endif