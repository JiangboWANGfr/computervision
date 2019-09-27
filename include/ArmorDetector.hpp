/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:26:02 +0800
 * @LastEditTime: 2019-09-19 03:30:11
 * @LastEditors: 
 * @Description: 
 */
#ifndef ARMORDETECTOR_HPP
#define ARMORDETECTOR_HPP

#include "header.h"

class ArmorDetector
{
public:
#ifdef BLUE
    cv::Scalar huahuaScalar = cv::Scalar(255, 0, 0);
#endif
#ifdef RED
    cv::Scalar huahuaScalar = cv::Scalar(0, 0, 255);
#endif

private:
    vector<RotatedRect> rectLists, lightLists;
    vector<Point2f> armorCenters;
    vector<int> iRem;  //用于记录装甲板中心对应的两个灯条是x顺序中的哪个
    vector<int> jRem;  //同iRem
    vector<int> sbRem; //用于记录装甲板大小类型
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    int armorRem; //用于记录最后选中的装甲板在所有装甲板中的顺序

    TargetData armordata;

    Mat ipImg;//源图，图片内存空间相同，但是换了个名称
    Mat edges;

public:
    int mode; //用于判断是识别装甲板还是能量机关
    ArmorDetector();
    void getCenters(cv::Mat &source_img, TargetData &armor_data);

private:
    void initializeVariate();
    int getContours();
    void colorThres(cv::Mat &opImage);
    void getLights(void);
    void getArmors(cv::Mat &ipImg);
    void getArmor(cv::Mat &ipImg);
    static inline bool angleCmp(cv::RotatedRect &angle1, cv::RotatedRect &angle2);
    static inline bool sizeCmp(cv::RotatedRect &size1, cv::RotatedRect &size2);
    static inline bool rxCmp(cv::RotatedRect &rx1, cv::RotatedRect &rx2);
    void xCmp(cv::RotatedRect &x1, cv::RotatedRect &x2);
    int whiteSums(cv::Mat &src);
    void drawRRects(cv::Mat &ipImg, std::vector<cv::RotatedRect> RectVectors);
    bool judgeRectangleIsRight(const RotatedRect &rRect, const float &rRectArea);
};

#endif
