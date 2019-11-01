/*
 * @Copyright: CS of BIT
 * @Author: 孟文珩
 * @File name: 
 * @Version: 
 * @Date: 2019-10-26 19:20:55
 * @LastEditTime: 2019-10-31 22:08:54
 * @LastEditors: 
 * @Description: 
 */
#ifndef _INFANTRY_ARMOR_DETECTOR_H_
#define _INFANTRY_ARMOR_DETECTOR_H_

#include "header.h"
#include "ArmorDetector.h"

#ifdef INFANTRY

class InfantryArmorDetector : public ArmorDetector
{
private:
    Mat src;//source image
    Mat binary;//binary image
    Mat outline;//outline image
    Mat roiimg;
    vector< vector<Point> > contours;
    int num;
    float matchrank[1500][1500];
    float dis[1500][1500];
    bool isbig;

private:
    typedef struct MODE_PARAM
    {
        float relative_position_radio;
        int relative_angle[4][2];
        float relative_area[5];
        float center_distance[2][2];
    }ModeParam;

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
    void debugGetCenter(Mat &src, TargetData &armor_data);
    void pickBest();
    void lostArmor();

private:
    typedef struct ROI
    {
        Point2f lefttop;
        int rwidth;
        int rheight;
        int level;
    }Roi;
    Roi roi;
    typedef struct ARMOR
    {
        Point2f center;
        Point2f rect[4];
    }Armor;
    Armor target;
    int besti = -1,bestj = -1;
    int maxpoint=-8000;
    RotatedRect boxi;
    RotatedRect boxj;

private:
    void getBinaryImage();
    void getContours();
    void getTarget();
    void getPnP();

public: 
    InfantryArmorDetector();
    InfantryArmorDetector(Mat &src);
    void getCenter(cv::Mat &source_img, TargetData &armor_data);

};




#endif
#endif
