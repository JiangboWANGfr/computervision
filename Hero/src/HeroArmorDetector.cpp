/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-10-07 00:51:35
 * @LastEditTime : 2019-12-28 17:05:38
 * @LastEditors  : zzdr
 */
#ifndef HeroArmorDetector_H
#define HeroArmorDetector_H
#include "HeroAngleCalculate.hpp"
#include "HeroArmorDetector.hpp"
#ifdef Hero
using namespace std;
using namespace cv;

//初始化
void HeroArmorDetector::setup()
{
    srcImage = cv::Mat::zeros(1280, 1024, CV_8UC3);
    thresholdImageRed = cv::Mat::zeros(1280, 1024, CV_8UC3);
    thresholdImageBlue = cv::Mat::zeros(1280, 1024, CV_8UC3);

    BrighttoCannyContours.resize(0);
    BrighttoCannyHierarchy.resize(0);
    chasecenter.resize(0);
    chasedistance.resize(0);
    armor_frame.resize(0);
    filteredcenter.resize(0);
    armorDetectionLeft.resize(0);
    armorDetectionRight.resize(0);
    rgbChannels.resize(3);
    R_B = false;
    B_S = true;
    armor_location = false;
    pre_pt.x = 0;
    pre_pt.y = 0;
    pnpresult.pitch = 0;
    pnpresult.yaw = 0;
    pnpresult.x = 0;
    pnpresult.y = 0;
    pnpresult.z = 0;
    pnpresult.find_armor = false;
    target.center.x = 0;
    target.center.y = 0;
    width = 640;
    height = 480;

    Kern = (Mat_<char>(3, 3) << 1, 1, 1,
            1, 1, 1,
            1, 1, 1); //卷积核 具有增强效果
}
// //裁剪图片，得到chasecenter表示中心点
// void HeroArmorDetector::cut(cv::Mat &g_srcImage)
// {
//     cout<<g_srcImage.size()<<endl;
//     //cv::resize(g_srcImage,g_srcImage,Size(1280,1080));
//     cout<<g_srcImage.size()<<endl;
//     cv::Mat cutImage;
//     cv::Point2i center_point;
//     cv::Point2i cur_pt;
    
//     if (armor_location)
//     {
//         center_point.x = chasecenter.back().x;
//         center_point.y = chasecenter.back().y;
//         width = 640;
//         height = 480;
//     }
//     else
//     {
//         center_point.x = 640;
//         center_point.y = 512;
//         width = 1280;
//         height = 1024;
//     }

//     srcImage.release();
//     thresholdImageRed.release();
//     thresholdImageBlue.release();

//     cur_pt.x = center_point.x + width / 2;
//     cur_pt.y = center_point.y + height / 2;
//     pre_pt.x = center_point.x - width / 2;
//     pre_pt.y = center_point.y - height / 2;

//     cout << "17mm center_point:" << center_point << endl;

//     if (pre_pt.x < 0)
//     {
//         pre_pt.x = 0;
//     }
//     if (pre_pt.y < 0)
//     {
//         pre_pt.y = 0;
//     }
//     if (cur_pt.x > 1280)
//     {
//         pre_pt.x = pre_pt.x - (cur_pt.x - 1280);
//     }
//     if (cur_pt.y > 1024)
//     {
//         pre_pt.y = pre_pt.y - (cur_pt.y - 1024);
//     }
  
//     srcImage = g_srcImage(Rect(pre_pt.x, pre_pt.y, width, height));
// }

//裁剪图片，得到chasecenter表示中心点
void HeroArmorDetector::cut(cv::Mat &g_srcImage)
{
    cout<<g_srcImage.size()<<endl;
    //cv::resize(g_srcImage,g_srcImage,Size(1280,1080));
    cout<<g_srcImage.size()<<endl;
    // cv::Point2i center_point;
    // cv::Point2i cur_pt;
    // center_point.x = chasecenter.back().x;
    // center_point.y = chasecenter.back().y;
    // width = 640;
    // height = 480;
    
    srcImage = g_srcImage;
}




//改变亮度并卷积加强边缘
void HeroArmorDetector::Bright()
{
    int alpha = 10;
    int nThresh = 200;
    //改变图像亮度
    cv::Mat Bright_image = srcImage;
    double Bright_alpha = alpha / 10;

    cv::Mat Bright_change;
    Bright_image.convertTo(Bright_change, -1, Bright_alpha, nThresh - 255);

    //分离通道
    cv::split(Bright_change, rgbChannels);
}

//改变亮度
void HeroArmorDetector::Bright(int alpha, int nThresh)
{
    //改变图像亮度
    cv::Mat Bright_image = srcImage;
    double Bright_alpha = alpha / 10;

    cv::Mat Bright_change;
    Bright_image.convertTo(Bright_change, -1, Bright_alpha, nThresh - 255);

    //分离通道
    cv::split(Bright_change, rgbChannels);

    //卷积处理 卷积核为Kern

    cv::filter2D(rgbChannels[0], rgbChannels[0], rgbChannels[0].depth(), Kern);

    cv::filter2D(rgbChannels[2], rgbChannels[2], rgbChannels[2].depth(), Kern);
}

//Canny边缘检测&寻找轮廓 ，得到的全部矩阵放在BrighttoCannyContours中
void HeroArmorDetector::BrighttoCanny()
{
    int CannyLowThreshold = 150;

    cv::threshold(rgbChannels[0], thresholdImageBlue, 160, 255, THRESH_BINARY);
    cv::threshold(rgbChannels[2], thresholdImageRed, 160, 255, THRESH_BINARY);

    //Canny边缘检测&寻找轮廓
    if (R_B)
    {
        cv::Canny(thresholdImageBlue, thresholdImageBlue, CannyLowThreshold, CannyLowThreshold * 3, 3);
        cv::findContours(thresholdImageBlue, BrighttoCannyContours, BrighttoCannyHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    }
    else
    {
        cv::Canny(thresholdImageRed, thresholdImageRed, CannyLowThreshold, CannyLowThreshold * 3, 3);
        cv::findContours(thresholdImageRed, BrighttoCannyContours, BrighttoCannyHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    }
}
//Canny处理并输出轮廓得到的全部矩阵放在BrighttoCannyContours中
void HeroArmorDetector::BrighttoCanny(int CannyLowThreshold)
{
    cv::threshold(rgbChannels[0], thresholdImageBlue, 160, 255, THRESH_BINARY);
    cv::threshold(rgbChannels[2], thresholdImageRed, 160, 255, THRESH_BINARY);

    //Canny边缘检测&寻找轮廓
    if (R_B)
    {
        cv::Canny(thresholdImageBlue, thresholdImageBlue, CannyLowThreshold, CannyLowThreshold * 3, 3);
        cv::findContours(thresholdImageBlue, BrighttoCannyContours, BrighttoCannyHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    }
    else
    {
        cv::Canny(thresholdImageRed, thresholdImageRed, CannyLowThreshold, CannyLowThreshold * 3, 3);
        cv::findContours(thresholdImageRed, BrighttoCannyContours, BrighttoCannyHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    }
}

//旋转矩形包围轮廓方便观察
void HeroArmorDetector::ShowAreaRect()
{
    
    cv::RotatedRect minRect;
    cv::Mat minAreaRectImage = cv::Mat::zeros(srcImage.size(), CV_8UC3);
    for (int j = 0; j < BrighttoCannyContours.size(); j++)
    {
        //最小旋转矩形包围轮廓并输出四顶点
        minRect = cv::minAreaRect(cv::Mat(BrighttoCannyContours[j]));
        cv::Point2f rect_points[4]; // The order is bottomRight, bottomLeft, topLeft, topRight
        minRect.points(rect_points);

        //画旋转矩形
        for (int k = 0; k < 4; k++)
        {
            cv::line(minAreaRectImage, rect_points[k], rect_points[(k + 1) % 4], cv::Scalar(255, 0, 255), 2);
        }
    }

    cv::namedWindow("minAreaRect", WINDOW_AUTOSIZE);
    cv::resize(minAreaRectImage,minAreaRectImage,srcImage.size()/3);
    cout<<"showalearect"<<endl;
    cv::imshow("minAreaRect", minAreaRectImage);
}

//计算矩形边的角度
double HeroArmorDetector::calculate_LineDegree(const cv::Point2f &firstPt, const cv::Point2f &secondPt)
{
    double curLineAngle = 0.0f;
    if (secondPt.x - firstPt.x != 0)
    {
        curLineAngle = std::atan(static_cast<double>(firstPt.y - secondPt.y) / static_cast<double>(secondPt.x - firstPt.x));
        if (curLineAngle < 0)
        {
            curLineAngle += CV_PI;
        }
    }
    else
    {
        curLineAngle = CV_PI / 2.0f; //90度
    }
    return curLineAngle * 180.0f / CV_PI;
}
//计算矩形的角度
double HeroArmorDetector::get_Rectdegree(const cv::RotatedRect box)
{
    double degree = 0.0f;
    cv::Point2f vertVect[4];
    box.points(vertVect);
    //line 1
    const double firstLineLen = (vertVect[1].x - vertVect[0].x) * (vertVect[1].x - vertVect[0].x) +
                                (vertVect[1].y - vertVect[0].y) * (vertVect[1].y - vertVect[0].y);
    //line 2
    const double secondLineLen = (vertVect[2].x - vertVect[1].x) * (vertVect[2].x - vertVect[1].x) +
                                 (vertVect[2].y - vertVect[1].y) * (vertVect[2].y - vertVect[1].y);
    if (firstLineLen > secondLineLen)
    {
        degree = calculate_LineDegree(vertVect[0], vertVect[1]);
    }
    else
    {
        degree = calculate_LineDegree(vertVect[2], vertVect[1]);
    }
    return degree;
}

//遍历轮廓初步筛选寻找合适的矩阵将符合标准的旋转矩阵另存在filteredRect
void HeroArmorDetector::Traverse_contours_toFindrightRect(cv::Point2f (&frect_points)[4], float &real_angle, int &counter)
{
    for (int m = 0; m < BrighttoCannyContours.size(); m++)
    {
        cv::RotatedRect minRotatedRect = cv::minAreaRect(BrighttoCannyContours[m]);
        minRotatedRect.points(frect_points);
        real_angle = get_Rectdegree(minRotatedRect);
        float width = minRotatedRect.size.width;
        float height = minRotatedRect.size.height;

        if (width > height)
        {
            std::swap(width, height);
        }

        //调取点参数
        cv::Point2i Contour_center;
        Contour_center = frect_points[0];

        //初步筛选旋转矩阵并将符合标准的旋转矩阵另存在filteredRect
        std::vector<float> s(7); //s = size 外形标准
        s[0] = height / width;
        s[1] = height * width;
        s[2] = minRotatedRect.angle;
        s[3] = real_angle;
        s[4] = std::sqrt(std::pow(frect_points[1].y - frect_points[2].y, 2) + std::pow(frect_points[1].x - frect_points[2].x, 2)); //real height
        s[5] = std::sqrt(std::pow(frect_points[1].y - frect_points[0].y, 2) + std::pow(frect_points[1].x - frect_points[0].x, 2)); //real width

        bool armor_exist;

        //长度 面积 角度初步筛选
        if ((s[0] < 20 || s[0] > 1.2) && (1) && (s[1] > 10) && (s[3] > 70 && s[3] < 110))
        {
            // cout << "s[3]" << s[3] << endl;
            //判断颜色
            if (R_B)
            {
                int red = rgbChannels[2].at<uchar>(Contour_center);
                if (red < 100)
                    armor_exist = true;
            }
            else
            {
                int blue = rgbChannels[0].at<uchar>(Contour_center);
                if (blue < 100)
                    armor_exist = true;
            }

            if (armor_exist)
            {
                armor_exist = false;
                for (int k = 0; k < 4; k++)
                {
                    cv::line(srcImage, frect_points[k], frect_points[(k + 1) % 4], cv::Scalar(255, 0, 0), 2);
                }
                filteredRect.push_back(minRotatedRect);
                counter++;
            }
        }
    }
}
//寻找合适的矩形对，得到armorDetectionLeft，armorDetectionRight
int HeroArmorDetector::Find_final_rectangle(int &counter)
{
    //寻找合适的矩形对
    int length;            //相对位置
    float height_quotient; //高度比
    float width_quotient;  //宽度比
    float a_min;
    float a_max;
    float b_min;
    float b_max;
    double a_real_angle;
    double b_real_angle;
    int theta_angle; //相对角度
    float line_angle;
    int Rects_number = 0;
    armorDetectionLeft.clear();
    armorDetectionRight.clear();
    if (counter > 1)
    {
        //冒泡
        for (int a = 0; a < counter; a++)
        {
            for (int b = a; b < counter; b++)
            {
                a_min = (filteredRect[a].size.height > filteredRect[a].size.width) ? filteredRect[a].size.width : filteredRect[a].size.height;
                a_max = (filteredRect[a].size.height > filteredRect[a].size.width) ? filteredRect[a].size.height : filteredRect[a].size.width;
                b_min = (filteredRect[b].size.height > filteredRect[b].size.width) ? filteredRect[b].size.width : filteredRect[b].size.height;
                b_max = (filteredRect[b].size.height > filteredRect[b].size.width) ? filteredRect[b].size.height : filteredRect[b].size.width;
                a_real_angle = get_Rectdegree(filteredRect[a]);
                b_real_angle = get_Rectdegree(filteredRect[b]);
                length = std::sqrt(std::pow(filteredRect[a].center.x - filteredRect[b].center.x, 2) + std::pow(filteredRect[a].center.y - filteredRect[b].center.y, 2));
                theta_angle = std::abs(a_real_angle - b_real_angle);
                height_quotient = b_max / a_max;
                width_quotient = b_min / a_min;

                if (length > a_min * 1.8 && length > b_min * 1.8 && length < a_max * 6 && length < b_max * 6

                    && (theta_angle < 12) && height_quotient < 2.5 && height_quotient > 0.4 && width_quotient < 2.5 && width_quotient > 0.4)
                {
                    if (abs(int(filteredRect[a].center.x - filteredRect[b].center.x)) != 0)
                        line_angle = std::abs(180 / CV_PI * int(filteredRect[a].center.y - filteredRect[b].center.y)) / std::abs(int(filteredRect[a].center.x - filteredRect[b].center.x));
                    else
                        line_angle = 90;

                    if ((line_angle + a_real_angle < 105 && line_angle + a_real_angle > 80) || (line_angle + b_real_angle < 105 && line_angle + b_real_angle > 80))
                    {
                        cv::line(srcImage, filteredRect[a].center, filteredRect[b].center, cv::Scalar(255, 255, 0), 2);
                        cv::circle(srcImage, filteredRect[a].center / 2 + filteredRect[b].center / 2, 4, cv::Scalar(0, 255, 0), 4);
                        Rects_number++;
                        if (filteredRect[b].center.x > filteredRect[a].center.x)
                        {
                            armorDetectionRight.push_back(filteredRect[b]);
                            armorDetectionLeft.push_back(filteredRect[a]);
                        }
                        else
                        {
                            armorDetectionRight.push_back(filteredRect[a]);
                            armorDetectionLeft.push_back(filteredRect[b]);
                        }
                    } //线->角度
                }     //匹配矩形对
            }         //冒泡
        }             //冒泡
    }
}
//找到装甲板中心 传入左灯条和右灯条，得到target.rect[0],rect[1],rect[2],rect[3] 装甲板的四个点
void HeroArmorDetector::find_targetpoints(cv::RotatedRect &armorlamppost_left, cv::RotatedRect &armorlamppost_right)
{
    cv::Point2f frect_points[4];
    std::vector<cv::Point2f> top_points(0);
    std::vector<cv::Point2f> bottom_points(0);
    armorlamppost_left.points(frect_points);
    for (int i = 0; i < 4; i++)
    {
        if (frect_points[i].y < armorlamppost_left.center.y)
            top_points.push_back(frect_points[i]);
        else
            bottom_points.push_back(frect_points[i]);
    }

    target.rect[0] = Point2i((top_points[0] + top_points[1]) / 2);
    target.rect[1] = Point2i((bottom_points[0] + bottom_points[1]) / 2);

    armorlamppost_right.points(frect_points);
    for (int i = 0; i < 4; i++)
    {
        if (frect_points[i].y < armorlamppost_right.center.y)
            top_points.push_back(frect_points[i]);
        else
            bottom_points.push_back(frect_points[i]);
    }
    target.rect[2] = Point2i((top_points[2] + top_points[3]) / 2);
    target.rect[3] = Point2i((bottom_points[2] + bottom_points[3]) / 2);
}
//得到装甲板中心的编号，filteredcenter表示装甲板中心的编号
void HeroArmorDetector::get_armorcenter_number(int &Rects_number)
{
    filteredcenter.push_back(Rects_number - 1); //最后一个中心点
    for (int p = 0; p < Rects_number - 1; p++)
    {
        for (int q = p + 1; q < Rects_number; q++)
        {
            cv::Point2i armor_center_p = armorDetectionLeft[p].center / 2 + armorDetectionRight[p].center / 2;
            cv::Point2i armor_center_q = armorDetectionLeft[q].center / 2 + armorDetectionRight[q].center / 2;
            if (std::abs(armor_center_p.x - armor_center_q.x) < 5 && std::abs(armor_center_p.y - armor_center_q.y) < 5)
            {
                break;
            }
            if (q == Rects_number - 1)
            {
                filteredcenter.push_back(p); //第p+1个
            }
        }
    }
}
//跟踪
//选一个最合适的中心点,得到chasecenter表示中心点
int HeroArmorDetector::get_armorcenter_point()
{
    int hh = 0;
    int score_h = 0;
    int score_hh = 0;
    for (int h = 1; h < filteredcenter.size(); h++)
    {
        //相对角度
        float delta_angle_h = get_Rectdegree(armorDetectionRight[filteredcenter[h]]) - get_Rectdegree(armorDetectionLeft[filteredcenter[h]]);
        float delta_angle_hh = get_Rectdegree(armorDetectionRight[filteredcenter[hh]]) - get_Rectdegree(armorDetectionLeft[filteredcenter[hh]]);
        //左右比例
        float right_height_h = (armorDetectionRight[filteredcenter[h]].size.height > armorDetectionRight[filteredcenter[h]].size.width) ? armorDetectionRight[filteredcenter[h]].size.height : armorDetectionRight[filteredcenter[h]].size.width;
        float left_height_h = (armorDetectionLeft[filteredcenter[h]].size.height > armorDetectionLeft[filteredcenter[h]].size.width) ? armorDetectionLeft[filteredcenter[h]].size.height : armorDetectionLeft[filteredcenter[h]].size.width;
        float height_quotient_h = right_height_h / left_height_h;
        if (height_quotient_h < 1.0)
            height_quotient_h = 1.0 / height_quotient_h;

        float right_height_hh = (armorDetectionRight[filteredcenter[hh]].size.height > armorDetectionRight[filteredcenter[hh]].size.width) ? armorDetectionRight[filteredcenter[hh]].size.height : armorDetectionRight[filteredcenter[hh]].size.width;
        float left_height_hh = (armorDetectionLeft[filteredcenter[hh]].size.height > armorDetectionLeft[filteredcenter[hh]].size.width) ? armorDetectionLeft[filteredcenter[hh]].size.height : armorDetectionLeft[filteredcenter[hh]].size.width;
        float height_quotient_hh = right_height_hh / left_height_hh;
        if (height_quotient_hh < 1.0)
            height_quotient_hh = 1.0 / height_quotient_hh;
        //面积
        float area_h = armorDetectionRight[filteredcenter[h]].size.width * armorDetectionRight[filteredcenter[h]].size.height +
                    armorDetectionLeft[filteredcenter[h]].size.width * armorDetectionLeft[filteredcenter[h]].size.height;
        float area_hh = armorDetectionRight[filteredcenter[hh]].size.width * armorDetectionRight[filteredcenter[hh]].size.height +
                        armorDetectionLeft[filteredcenter[hh]].size.width * armorDetectionLeft[filteredcenter[hh]].size.height;
        cout << "area_h = " << area_h << "\t"
            << "area_hh = " << area_hh << endl;
        if (abs(delta_angle_h) < abs(delta_angle_hh))
            score_h++;
        else
            score_hh++;
        if (height_quotient_h < height_quotient_hh)
            score_h++;
        else
            score_hh++;
        if (area_h > area_hh)
            score_h += 3;
        else
            score_hh += 3;

        if (score_h > score_hh)
        {
            hh = h;
        }
    }
    cv::circle(srcImage, armorDetectionRight[filteredcenter[hh]].center / 2 + armorDetectionLeft[filteredcenter[hh]].center / 2, 8, cv::Scalar(255, 0, 255), 8);
    //目标压入
    HeroArmorDetector::find_targetpoints(armorDetectionLeft[filteredcenter[hh]], armorDetectionRight[filteredcenter[hh]]);
    //保存中心点
    if (chasecenter.size() < 10) //只保留后 x 个元素
    {
        chasecenter.push_back(cv::Point2i(armorDetectionLeft[filteredcenter[hh]].center / 2 + armorDetectionRight[filteredcenter[hh]].center / 2) + pre_pt);
    }
    else
    {
        chasecenter.erase(chasecenter.begin());
        chasecenter.push_back(cv::Point2i(armorDetectionLeft[filteredcenter[hh]].center / 2 + armorDetectionRight[filteredcenter[hh]].center / 2) + pre_pt);
    }
    return hh;
}
//旋转矩形包围轮廓并筛选出合适的矩形对(图像处理主函数)
void HeroArmorDetector::filter()
{
    AngleCalculate pnpor;
    cv::Point2f frect_points[4];
    float real_angle;
    int counter = 0;
    cout<<"go into filter"<<endl;
    ////遍历轮廓初步筛选寻找合适的矩阵将符合标准的旋转矩阵另存在filteredRect
    Traverse_contours_toFindrightRect(frect_points, real_angle, counter);
    ////寻找合适的矩形对，得到armorDetectionLeft，armorDetectionRight，
    int Rects_number = Find_final_rectangle(counter);
    filteredcenter.clear();
    if (!armorDetectionLeft.empty())
    {
        cout<<"findarmorlight"<<endl;
        armor_location = true; //是否找到了装甲板中心
        armor_frame.push_back(true);
        //得到的结果中filteredcenter表示装甲板中心的编号
        get_armorcenter_number(Rects_number);
        //选一个最合适的中心点,得到chasecenter表示中心点
        int hh = get_armorcenter_point();
       pnpor.pnpSolver(pnpresult,hh,target,pre_pt);
        ShowAreaRect();
    }
    else
    {
        NoArmorFound_toDO_();
    }
}

void HeroArmorDetector::NoArmorFound_toDO_()
{
    cout << " no armor found 17 mm !" << endl;
    armor_frame.push_back(false);
    pnpresult.pitch = 0.0;
    pnpresult.yaw = pnpresult.yaw * 0.95;
    pnpresult.x = 0.0;
    pnpresult.y = 0.0;
    pnpresult.z = 0.0;
    pnpresult.find_armor = false;
    if (armor_frame.size() > 3)
        for (int s = 0; s < 3; s++)
        {
            if (armor_frame[armor_frame.size() - 1 - s] == true)
                break;
            if (s == 2)
            {
                armor_location = false;
                pnpresult.find_armor = false;
            }
        }
    else
    {
        armor_location = false;
        pnpresult.find_armor = false;
    }
}
bool HeroArmorDetector::xCmp(float &x1, float &x2)
{
    return x1 < x2; //从小到大排序
}

  void HeroArmorDetector::getCenter(cv::Mat &source_img, TargetData &armor_data){
    setup();
    cut(source_img);
    Bright();
    BrighttoCanny();
    filter();
    armor_data.pitch_angle = pnpresult.pitch * 1 * -1 + pnpresult.z * 0.0005;
    armor_data.yaw_angle =pnpresult.yaw * 1 * 1;
    armor_data.x = pnpresult.x;
    armor_data.y = pnpresult.y;
    armor_data.z = pnpresult.z;
    armor_data.is_get =pnpresult.find_armor;
  }
#endif // HeroArmorDetector_H
#endif