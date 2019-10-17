/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-10-07 20:33:23
 * @LastEditTime: 2019-10-10 10:10:49
 * @LastEditors: Please set LastEditors
 */
#ifndef _SIVIRDETECTOR_H_
#define _SIVIRDETECTOR_H_
#include<opencv2/opencv.hpp>
#include<vector>
#include<math.h>
#include"config.h"
#include "header.h"

using namespace cv;
using namespace std;

typedef struct ARMOR2
{
    Point2f center;
    Point2f rect[4];
}Armor2;

typedef struct ANGLE2
{
    float yaw;
    float pitch;
}Angle2;

typedef struct SCIRCLE2
{
    Point2f center;
    float r;
}Scircle2;

class SivirDetector
{
public:
    Mat src;//source image
    Mat binary;//binary image
    Mat outline;//outline image
    bool islost;
    vector< vector<Point> > contours;
    int num;
    Point2f small[100];
    Point2f big[100];
    int cntsmall;
    int cntbig;
    Armor2 target;
    Angle2 pnpresult;
    Point2f allrect[100][4];
    
    Point2f predict[400];
    int prep;
    Scircle2 heart;
    
    Point2f scCenter;

    int mode;
    int clockwise;
public: 
    SivirDetector();
    SivirDetector(Mat src0);
    void getResult(Mat src0);
    void getSrcImage(Mat src0);
    void getBinaryImage();
    void getContours();
    void getTarget();
    void getPnP();
    void getCenter(vector<vector <Point> > &fcContours,RotatedRect &rRect);
    void getRectangle(vector<RotatedRect> &box,Point2f rect[4],vector<Rect> &boundRect);
    void getRectangleCenter(vector<RotatedRect> &box);
    void NoTarget();
    void getTargetCoordinateMode1();
    void getTargetCoordinateMode2(int flag[20]);
    void setCamera(Mat  &_A_matrix,Mat &distCoeffs);
    void waitPreparation();
    void attackingStationaryTarget();
    void calculatingCoordinates(Mat &_R_matrix,Mat &_t_matrix);
};

#endif
