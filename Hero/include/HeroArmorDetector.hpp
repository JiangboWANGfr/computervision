/*
 * @Descripttion: 
 * @version: 
 * @Author: zzdr
 * @Date: 2019-10-19 09:24:15
 * @LastEditors: zzdr
 * @LastEditTime: 2019-10-22 07:28:09
 */

#ifndef ARMORDETECTOR_HPP
#define ARMORDETECTOR_HPP
#include "header.h"
#include "ArmorDetector.h"

#ifdef Hero
using namespace std;
using namespace cv;

typedef struct ARMOR
{
    cv::Point2f center;
    cv::Point2f rect[4];
} Armor;

typedef struct ANGLE	//cout << s[0] << endl;
{
    float yaw;
    float pitch;
    float x;
    float y;
    float z;
    bool find_armor;
} Angle;
class HeroArmorDetector:public ArmorDetector
{
public:
    bool R_B;
    cv::Mat srcImage;
    Armor target;
    Angle pnpresult;
    void getCenter(cv::Mat &source_img, TargetData &armor_data);
    
private:
    std::vector<cv::Mat> rgbChannels;
    std::vector<std::vector<cv::Point> > BrighttoCannyContours;
    std::vector<cv::Vec4i> BrighttoCannyHierarchy;
    cv::Mat thresholdImageRed;
    cv::Mat thresholdImageBlue;
    std::vector<cv::RotatedRect> armorDetectionLeft;
    std::vector<cv::RotatedRect> armorDetectionRight;
    std::vector<int> filteredcenter;
    std::vector<bool> armor_frame;
    std::vector<cv::Point2i> chasecenter;
    std::vector<int> chasedistance;
    int lens = 9;
    float disMatrix[9] = {0};
    bool armor_location;
    bool B_S;
    cv::Point2i pre_pt;
    int width;
    int height;
    std::vector<cv::RotatedRect> filteredRect;
    cv::Mat Kern;

private: 
    void setup();
    void cut(cv::Mat &g_srcImage);
    void Bright();
    void Bright(int alpha, int nThresh);
    void BrighttoCanny();
    void BrighttoCanny(int CannyLowThreshold);
    void filter();
    void ShowAreaRect();
    void find_targetpoints(cv::RotatedRect &armorlamppost_left, cv::RotatedRect &armorlamppost_right);
    double calculate_LineDegree(const cv::Point2f &firstPt, const cv::Point2f &secondPt);
    double get_Rectdegree(const cv::RotatedRect box);
    static inline bool xCmp(float &x1, float &x2);
    void SetParameter_ForPnpSover(std::vector<cv::Point3f> &centerpoint_in_realworld_axis, std::vector<cv::Point2f> &points, cv::Mat &_A_matrix, cv::Mat &distCoeffs);
    void calculate_pitch_and_yaw(double &newx, double &newy, double &newz);
    void Traverse_contours_toFindrightRect(cv::Point2f (&frect_points)[4], float &real_angle, int &counter) ;
    int  Find_final_rectangle(int &counter);
    void get_armorcenter_number(int &Rects_number);
    int  get_armorcenter_point();
    void Calculate_x_y_z_in_camera_axis(int &hh, cv::Mat &_R_matrix,cv::Mat &_t_matrix,double &newx,double &newy,double &newz);
    void NoArmorFound_toDO_();  
};

#endif
#endif
