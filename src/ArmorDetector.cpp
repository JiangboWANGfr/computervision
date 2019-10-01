/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:33:02 +0800
 * @LastEditTime: 2019-09-29 07:31:29
 * @LastEditors: 
 * @Description: 
 */
#include "ArmorDetector.hpp"
#include "AngleCalculate.hpp"
#include "actions.h"

ArmorDetector::ArmorDetector()
{
    initializeVariate();
}

void ArmorDetector::initializeVariate()
{
    armordata.is_get = 0;
    armordata.shoot = 0;
    rectLists.clear();
    lightLists.clear();
    armorCenters.clear();
    iRem.clear();
    jRem.clear();
    sbRem.clear();
    armorRem = 0;
}

bool ArmorDetector::judgeRectangleIsRight(const RotatedRect &rRect, const float &rRectArea)
{
    if (((fabs(rRect.angle) < 45.0 && rRect.size.height > rRect.size.width) ||
         (fabs(rRect.angle) > 45.0 && rRect.size.width > rRect.size.height)) &&
        rRectArea > 12 && rRectArea < 10000)
    {
        return true;
    }
    return false;
}

void ArmorDetector::getCenters(cv::Mat &source_img, TargetData &armor_data)
{
    ipImg = source_img;
    // 全局变量初始化
    initializeVariate();

    getContours();

    // 一轮粗筛选
    // rRectArea的最小面积越小代表识别距离越远，但噪声图形变多的可能性增大
    RotatedRect rRect;
    for (int i = 0; i < contours.size(); i++)
    {
        rRect = minAreaRect(contours[i]);
        float rRectArea = rRect.size.width * rRect.size.height;
        if (judgeRectangleIsRight(rRect, rRectArea))
        {
            //像素点少时，fitEllipse对灯条角度的拟合效果要比minAreaRect好一点
            if (contours[i].size() >= 6)
            {
                RotatedRect box = fitEllipse(contours[i]);
                ellipse(ipImg, box, Scalar(0, 255, 0), 1, 8);
                if (box.angle > 90)
                {
                    box.angle = box.angle - 180;
                }
                else if (box.angle <= 90 && box.angle > 0)
                {
                    box.angle = box.angle - 90;
                }
                else
                {
                    box.angle = box.angle;
                }

                // minAreaRect和fitEllipse角度相差不大时选fitEllipse来获得需求的精度，但相差较大时使用minAreaRect会更简洁粗暴
                if (abs(rRect.angle - box.angle) < 13 || abs(rRect.angle - box.angle) > 80) //如果理解RotatedRect的话，会发现-0和-90是相近的
                {
                    rRect.angle = box.angle;
                }
            }
            rectLists.push_back(rRect);
        }
    }
    if (rectLists.size() < 2)
    {
        return;
    }
    sort(rectLists.begin(), rectLists.end(), rxCmp);

    // 获得所有灯条
    getLights();
    if (lightLists.size() < 2)
    {
        return;
    }
    sort(lightLists.begin(), lightLists.end(), rxCmp);

#ifdef DEBUG1
    // 在ipImg上画出灯柱的包围矩形，并终端输出矩形数量
    //cout << "lightLists = " << lightLists.size() << endl;
    drawRRects(ipImg, lightLists);
    cout << "**********************************************" << endl;
#endif

    // 获得所有装甲中心
    getArmors(ipImg);
    if (armorCenters.size() < 1)
    {
        return;
    }

#ifdef DEBUG1
    // 在ipImg中画出所有装甲中心
    // cout << "sizeCC = " << armorCenters.size() << endl;
    for (int c = 0; c < armorCenters.size(); c++)
    {
        circle(ipImg, armorCenters[c], 10, huahuaScalar, 2, 8, 0);
    }
#endif

    // 决策唯一装甲
    getArmor(ipImg);
    armordata.is_get = 1;
    Point2f armorCenter;
    armorCenter = Point2f(0.5 * (lightLists[iRem[armorRem]].center.x + lightLists[jRem[armorRem]].center.x),
                          0.5 * (lightLists[iRem[armorRem]].center.y + lightLists[jRem[armorRem]].center.y));
#ifdef DEBUG1
    circle(ipImg, armorCenter, 15, huahuaScalar, 2, 8, 0);
#endif

    // 计算三维信息
    AngleCalculate pnpor;
    armordata.is_big = sbRem[armorRem];
    pnpor.pnpSolver(lightLists[iRem[armorRem]], lightLists[jRem[armorRem]], armordata);

    armor_data = armordata;
}

int ArmorDetector::getContours()
{
     // 图像预处理并寻找轮廓
    colorThres(edges);
    showPicture("edge", edges, 0.001);
    findContours(edges, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
/*
        findContours搭配效果
        mode                    method                  then
        0,1,2,3                 1,2,3,4                 (0,2or3or4)
        0简洁,1,2,3点留下噪声点    1的点太多，2中，3,4少
        method选用2时旋转矩形会出现斜图变直现象，3or4虽为斜图但斜的方向与灯条相反
        此版本选用(0,2)即RETR_EXTERNAL和CHAIN_APPROX_SIMPLE
    */

   
}

// 灯条为红蓝且高亮
// 此版图像处理在处理红色灯条时高亮部分阈值容易受现场亮度影响，分区赛光比复活赛亮，阈值取值不同
// 待优化：19年全国赛采用的是青橙灯效的地面，该算法未涉及把青橙和红蓝区分开
/**
 * @Author: 王占坤
 * @Description: 提取灯条边界
 * @Param: Mat ipImage  输入的图片
 * @Param: Mat opImage  处理后的输出图片
 * @Return: 
 * @Throw: 
 */
void ArmorDetector::colorThres(cv::Mat &opImage)
{
    Mat thres_whole;
    vector<Mat> splited;
    Mat binary;
    vector<vector<Point>> contours;
    Mat contourKernel = getStructuringElement(MORPH_ELLIPSE, Size(15, 15));
    Mat grayKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    split(ipImg, splited);
    cvtColor(ipImg, thres_whole, COLOR_BGR2GRAY);
#ifdef BLUE
    threshold(thres_whole, thres_whole, 100, 255, THRESH_BINARY); //复活赛用的100
    subtract(splited[0], splited[2], binary);
#endif
#ifdef RED
    threshold(thres_whole, thres_whole, 60, 255, THRESH_BINARY); //复活赛场地暗，适应性训练录视频后调低了点阈值，一般来说红的值会小于蓝的
    subtract(splited[2], splited[0], binary);
    //复活赛地面和麦轮及其他金属件反光发黄，出现哨兵射击地面和队友麦轮的现象，通过绿通道去除反光
    threshold(splited[1], splited[1], 100, 255, THRESH_BINARY);
    binary = binary - (binary & splited[1]);
#endif
    threshold(binary, binary, 150, 255, THRESH_BINARY); //150
    dilate(binary, binary, contourKernel);              //膨胀内核整的这么大是为了保证无论云台转动多快binary都能包含thres_whole
    dilate(thres_whole, thres_whole, grayKernel);       //识别极限由最小面积决定，膨胀让灯条变大，增加识别距离，但原理上会对pnp解算距离有影响
    // imshow("binary",binary);
    // imshow("thres_whole",thres_whole);
    opImage = binary & thres_whole; //红或者蓝区域&高亮区域
}

// 离得远的矩形分成不同组，离得近的矩形分为一组，然后同组内选面积最大者作为代表矩形
void ArmorDetector::getLights()
{
    RotatedRect rGMax;
    rGMax = rectLists[0];
    int cellMaxSize;
    int maxSize;
    cellMaxSize = rectLists[0].size.width * rectLists[0].size.height;
    for (int i = 1; i < rectLists.size(); i++)
    {
        if ((rectLists[i].center.x - rectLists[i - 1].center.x < 10) && (abs(rectLists[i].center.y - rectLists[i - 1].center.y) < 30)) //添加y向距离为了防止地面反光面积大于灯条面积而成为占领代表之位
        {
            maxSize = rectLists[i].size.height * rectLists[i].size.width;
            if (maxSize > cellMaxSize)
            {
                cellMaxSize = maxSize;
                rGMax = rectLists[i];
            }
        }
        else
        {
            lightLists.push_back(rGMax);
            rGMax = rectLists[i];
            cellMaxSize = rectLists[i].size.height * rectLists[i].size.width;
        }
    }
    lightLists.push_back(rGMax); //最后一个矩形在上面else中没进lightLists
}

// 得到所有可能的装甲板并分为大小两类
void ArmorDetector::getArmors(cv::Mat &ipImg)
{
    Point2f LC1, LC2;
    float llDS, hhHeight, ssHeight, llA;
    float llWHRitio, hhRitio, chRitio;

    for (int k = 0; k < lightLists.size(); k++)
    {
        int height = lightLists[k].size.height;
        int width = lightLists[k].size.width;
        if (height < width)
        {
            lightLists[k].size.height = width;
            lightLists[k].size.width = height;
        }
    }

    for (int i = 0; i < lightLists.size() - 1; i++)
    {
        LC1 = lightLists[i].center;
        for (int j = i + 1; j < lightLists.size(); j++)
        {
            LC2 = lightLists[j].center;
            llDS = sqrt((LC2.x - LC1.x) * (LC2.x - LC1.x) + (LC2.y - LC1.y) * (LC2.y - LC1.y));
            hhHeight = lightLists[i].size.height > lightLists[j].size.height ? lightLists[i].size.height : lightLists[j].size.height;
            ssHeight = lightLists[i].size.height < lightLists[j].size.height ? lightLists[i].size.height : lightLists[j].size.height;
            llWHRitio = llDS / hhHeight;             // 宽高比，根据大小装甲板划开两个可行区域，实际正对时比值分别为4和2.4
            hhRitio = hhHeight / ssHeight;           // 高高比
            chRitio = abs(LC1.y - LC2.y) / ssHeight; // 高度错开比
            // 角度值的处理，原始值为-0~-90，处理后
            lightLists[i].angle = lightLists[i].angle > -45 ? lightLists[i].angle : lightLists[i].angle + 90;
            lightLists[j].angle = lightLists[j].angle > -45 ? lightLists[j].angle : lightLists[j].angle + 90;
            llA = abs(lightLists[i].angle - lightLists[j].angle); // 角度差

            float whR1 = 1.5;
            float whR2 = 2.95;
            float whR3 = 3.2; //3
            float whR4 = 5.5; //4.4
            float chR1 = 1;
            float chR2 = 1.7; //错开比是跟宽高比相关的，大装甲板宽高比更大，错开比范围更大
#ifdef DEBUG3
            cout << "llA" << llA << endl;
            cout << "llWHRitio = " << llWHRitio << endl
                 << "hhRitio" << hhRitio << endl
                 << "chRitio" << chRitio << endl;
            cout << endl;
#endif

            // 由于哨兵500发弹丸有点多，为了让哨兵更暴力一点，而把各参数范围放大，即使出现误识别
            if (llWHRitio >= whR1 && llWHRitio <= whR2)
            {
                if (hhRitio < 1.35 && chRitio < chR1 && llA < 10)
                {
                    armorCenters.push_back(Point2f(0.5 * (LC1.x + LC2.x), 0.5 * (LC1.y + LC2.y)));
                    iRem.push_back(i);
                    jRem.push_back(j);
                    sbRem.push_back(0);
                    // cout << "small small small" << endl;
                }
            }
            if (llWHRitio >= whR3 && llWHRitio <= whR4)
            {
                if (hhRitio < 1.35 && chRitio < chR2 && llA < 10)
                {
                    armorCenters.push_back(Point2f(0.5 * (LC1.x + LC2.x), 0.5 * (LC1.y + LC2.y)));
                    iRem.push_back(i);
                    jRem.push_back(j);
                    sbRem.push_back(1);
                    // cout << "big big big" << endl;
                }
            }
        }
    }
}

// 选择灯条对总面积最大者作为打击目标，因为用距离判定的话还得逐一解算计算量会上去
// 下面这个版本只是测试版，复活赛还是用的最大面积进行比较
// 待优化：如何防止最终选取的目标几个差不多的装甲板之间跳动???
void ArmorDetector::getArmor(cv::Mat &ipImg)
{
    float maxScore, newScore;
    armorRem = 0;
    float arArea = lightLists[iRem[0]].size.width * lightLists[iRem[0]].size.height + lightLists[jRem[0]].size.width * lightLists[jRem[0]].size.height;
    float llAngle = abs(lightLists[iRem[0]].angle - lightLists[jRem[0]].angle); //角度差作为减分项
    maxScore = arArea - llAngle * 5 + sbRem[0] * 5;                             //大装甲板作为加分项,但不宜过大
    for (int k = 1; k < armorCenters.size(); k++)
    {
        arArea = lightLists[iRem[k]].size.width * lightLists[iRem[k]].size.height + lightLists[jRem[k]].size.width * lightLists[jRem[k]].size.height;
        llAngle = abs(lightLists[iRem[k]].angle - lightLists[jRem[k]].angle);
        newScore = arArea - llAngle * 5 + sbRem[k] * 5;
        if (newScore > maxScore)
        {
            armorRem = k;
            maxScore = newScore;
        }
    }
}

bool ArmorDetector::angleCmp(cv::RotatedRect &angle1, cv::RotatedRect &angle2) //暂无使用，暂保留
{
    return angle1.angle < angle2.angle;
}

bool ArmorDetector::sizeCmp(cv::RotatedRect &size1, cv::RotatedRect &size2) //暂无使用，暂保留
{
    return size1.size.width * size1.size.height > size2.size.width * size2.size.height;
}

bool ArmorDetector::rxCmp(cv::RotatedRect &rx1, cv::RotatedRect &rx2)
{
    return rx1.center.x < rx2.center.x;
}

void ArmorDetector::xCmp(cv::RotatedRect &x1, cv::RotatedRect &x2) //暂无使用，暂保留
{
    vector<RotatedRect> campRect(2);
    campRect[0] = x1.center.x < x2.center.x ? x1 : x2;
    campRect[1] = x1.center.x > x2.center.x ? x1 : x2;
    x1 = campRect[0];
    x2 = campRect[1];
}

int ArmorDetector::whiteSums(cv::Mat &src)
{

    int counter = 0;
    //迭代器访问像素点
    Mat_<uchar>::iterator it = src.begin<uchar>();
    Mat_<uchar>::iterator itend = src.end<uchar>();
    for (; it != itend; ++it)
    {
        if ((*it) > 0)
            counter += 1; //二值化后，像素点是0或者255
    }
    return counter;
}

void ArmorDetector::drawRRects(cv::Mat &ipImg, std::vector<cv::RotatedRect> RectVectors)
{
    for (int k = 0; k < RectVectors.size(); k++)
    {
        Point2f rect[4];
        RectVectors[k].points(rect);
        for (int dd = 0; dd < 4; dd++)
        {
            line(ipImg, rect[dd], rect[(dd + 1) % 4], huahuaScalar, 2, 8);
        }
    }
}

// ToDoLists:
// 1.试试YUV，优化现场调参方便性及多种环境的鲁棒性
