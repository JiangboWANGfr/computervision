/*
 * @Description: 识别能量机关
 * @Author: chen
 * @Date: 2019-09-24 21:55:58
 * @LastEditTime: 2019-10-10 10:09:25
 * @LastEditors: Please set LastEditors
 */
#include "SivirDetector.h"
#define PI 3.14159265
using namespace cv;
/**
 * @description: 基本设置，包括模式设置
 * @param {type} no
 * @return: no
 */
SivirDetector::SivirDetector()
{
    islost=true;
    prep=0;
    mode=1;
    clockwise=0;
    heart.center.x=0;
    heart.center.y=0;
    heart.r=0;
}
/**
 * @description: 基本设置，包括模式设置
 * @param {type} no
 * @return: no
 */
SivirDetector::SivirDetector(Mat src0)
{
    src0.copyTo(src); 
    islost=true;
    mode=1;
    prep=0;
    clockwise=0;
    heart.center.x=0;
    heart.center.y=0;
    heart.r=0;
}
/**
 * @description: 识别能量机关的整个流程的汇总
 * @param {type} src0：复制原图
 * @return: no
 */
void SivirDetector::getResult(Mat src0)
{
    for (int i=0;i<10;i++)
    {
        small[i]=Point2f(0,0);
        big[i]=Point2f(0,0);
    }
    target.center.x=0;
    target.center.y=0;
    getSrcImage(src0);
    getBinaryImage();
    //imshow("bin",binary);
    getContours();
    getTarget();
    //imshow("res",src);
    getPnP();
    imshow("last",src);
    return;
}

/**
 * @description: 复制原图
 * @param {type} src0：复制原图
 * @return: no
 */
void SivirDetector::getSrcImage(Mat src0)
{
    src0.copyTo(src);
    return;
}

/**
 * @description: 二值化
 * @param {type} no
 * @return: no
 */
void SivirDetector::getBinaryImage()
{
    Mat gry;
    src.copyTo(gry);
    int isred=1;
    if (isred)
    {
        vector<Mat> imgChannels;
        split(src,imgChannels);
        gry=imgChannels.at(2)-imgChannels.at(0);
        threshold(gry,binary,50,255,CV_THRESH_BINARY);
    }   
    else
    {
        vector<Mat> imgChannels;
        split(src,imgChannels);
        gry=imgChannels.at(0)-imgChannels.at(2);
        threshold(gry,binary,50,255,CV_THRESH_BINARY);
    }
}
/**
 * @description: 获得能量机关灯条的矩形，提取信息，为接下来的筛选准备
 * @param {type} no
 * @return: no
 */
void SivirDetector::getContours()
{
    //ZJH
    Mat grayKernel = getStructuringElement(MORPH_ELLIPSE,Size(7,7));
    Mat fcImage = binary.clone();
    dilate(fcImage,fcImage,grayKernel);         //为了使流水灯与装甲板连接成为大面积矩形从而与圆心Ｒ区分开
    //imshow("thres",fcImage);
    vector<vector <Point> > fcContours;
    vector<Vec4i> fcHierarchy;
    findContours(fcImage,fcContours,fcHierarchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
    RotatedRect rRect;    
    getCenter(fcContours,rRect);

    //YY
    Point seedPoint=Point(0,0);
    floodFill(binary,seedPoint,Scalar(255,255,255));
    binary=~binary;
    //imshow("manshui",binary);
    
    vector<Vec4i> hierarcy;
    Point2f rect[4];
    src.copyTo(outline);
    findContours(binary, contours, hierarcy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    vector<Rect> boundRect(contours.size());
    vector<RotatedRect> box(contours.size());//最小外接矩形集合
    num=contours.size();
    getRectangle(box,rect,boundRect);
    
    getRectangleCenter(box);
    //cout<<cntsmall<<" "<<cntbig<<endl;
}
/**
 * @description: 获得击打能量机关的区域，筛选后标识在用户界面上（画点）
 * @param {type} no
 * @return: no
 */
void SivirDetector::getTarget()
{
    NoTarget();
    islost=false;
    int flag[20]={0};
    for (int i=0;i<cntsmall;i++)
    {
        for (int j=0;j<cntbig;j++)
        {
            double d;
            d=sqrt((small[i].x-big[j].x)*(small[i].x-big[j].x)+(small[i].y-big[j].y)*(small[i].y-big[j].y));
            cout<<i<<" "<<j<<" "<<d<<endl;
            if (d<100) 
            {
               flag[i]=1;
               break;
            }
        }
    }
    if (cntsmall==1)
    {
        getTargetCoordinateMode1();
    }
    else
    {
       getTargetCoordinateMode2(flag);
    }
    circle(src,Point(target.center.x,target.center.y),5,Scalar(0,0,255),-1,10);
    char tam[100]; 
	sprintf(tam, "(%0.0f,%0.0f)",target.center.x,target.center.y); 
    putText(src, tam, Point(target.center.x, target.center.y), FONT_HERSHEY_SIMPLEX, 1, cvScalar(255,0,255),1);
    return;
}
/**
 * @description: 获得能量机关的坐标，并判断该如何击打能量机关，包括考虑距离带来的时间延迟。
 * @param {type} no
 * @return: no
 */
void SivirDetector::getPnP()
{
    pnpresult.yaw=0;
    pnpresult.pitch=0;
   waitPreparation();
    // ZJH对半径的计算和装甲运动的预测
    // 由于炮口与大能量机关不在同一个平面，故不能使用该平面内的预测，姑且写一个平面的看看效果
    float sivirR = sqrt( (target.center.x - scCenter.x)*(target.center.x - scCenter.x) + \
                            (target.center.y - scCenter.y)*(target.center.y - scCenter.y) );
    heart.center.x = scCenter.x;
    heart.center.y = scCenter.y;
    heart.r = sivirR;
    if (mode == 9 ) //==1静止，不需要旋转
    {
        attackingStationaryTarget();
    }

    //控制点在世界坐标系中
    //按照顺时针圧入，左上是第一个点
    vector<Point3f> objP;
    Mat objM;
    objP.clear();
    objP.push_back(Point3f(0,0,0));
    objP.push_back(Point3f(240,0,0));
    objP.push_back(Point3f(240,100,0));
    objP.push_back(Point3f(0,100,0));
    Mat(objP).convertTo(objM,CV_32F);
    //目标四个点按照顺时针圧入，左上是第一个点
    vector<Point2f> points;
    points.clear();
    points.push_back(target.rect[0]);
    points.push_back(target.rect[1]);
    points.push_back(target.rect[2]);
    points.push_back(target.rect[3]);
    //设置相机内参和畸变系统
    Mat  _A_matrix = cv::Mat::zeros(3, 3, CV_64FC1);  
    Mat distCoeffs = cv::Mat::zeros(4, 1, CV_64FC1); 
    setCamera(_A_matrix,distCoeffs);
    //设置旋转、平移矩阵，旋转、平移向量
    Mat _R_matrix = cv::Mat::zeros(3, 3, CV_64FC1);   // rotation matrix
    Mat _t_matrix = cv::Mat::zeros(3, 1, CV_64FC1);   // translation matrix
    Mat rvec = cv::Mat::zeros(3, 1, CV_64FC1);          // output rotation vector
    Mat tvec = cv::Mat::zeros(3, 1, CV_64FC1);          // output translation vector
    solvePnP(objP,points,_A_matrix,distCoeffs,rvec,tvec);
    Rodrigues(rvec,_R_matrix);                   // converts Rotation Vector to Matrix
    _t_matrix = tvec;                            // set translation matrix
    //对应照相机3D空间坐标轴
    //求相机在世界坐标系中的坐标
    //求旋转矩阵的转置
   calculatingCoordinates(_R_matrix,_t_matrix);
}
/**
 * @description: 获得能量机关的中心
 * @param {type} fcContours：存储所有获得的矩形的容器
 *               rRect：存储最小的矩形，做筛选用的目标
 * @return: no
 */
void SivirDetector::getCenter(vector<vector <Point> > &fcContours,RotatedRect &rRect){
    for(int i=0; i < fcContours.size(); i++)
    {
        rRect = minAreaRect(fcContours[i]);
        float rRectArea = rRect.size.width*rRect.size.height;
        if(rRectArea > 300 && rRectArea < 700)
        {            
            //rectLists.push_back(rRect);  
            scCenter = rRect.center;        //大能量机关圆心,这里有一个bug就是该阈值必须只筛选出一个圆心，要不就呵呵了
            Point2f lRect[4];
            rRect.points(lRect);
            circle(src,scCenter,4,Scalar(0,255,0),-1,8); 
            //cout << "scCenter = " << scCenter << endl;                                            
        }
    }
}
/**
 * @description: 获得能量机关矩形灯条的边框
 * @param {type} box：最小外接矩形集合
 *               rect：存储矩形的点
 *               boundRect：存储轮廓的垂直边界最小矩形，用于存储图像一系列点的外部矩形边界。
 * @return: 
 */
void SivirDetector::getRectangle(vector<RotatedRect> &box,Point2f rect[4],vector<Rect> &boundRect){
    for (int i=0;i<num;i++)
    {
        box[i]=minAreaRect(Mat(contours[i]));//计算每个轮廓的最小外接矩形
        cout<<i<<":"<<box[i].size.area()<<endl;
        boundRect[i]=boundingRect(Mat(contours[i]));
        //circle(outline,Point(box[i].center.x,box[i].center.y),5,Scalar(0,255,0),-1,8);
        box[i].points(rect);
        //rectangle(outline, Point(boundRect[i].x, boundRect[i].y), Point(boundRect[i].x + boundRect[i].width, boundRect[i].y + boundRect[i].height), Scalar(0, 255, 0), 2, 8);
         for(int j=0; j<4; j++)
         {
            line(outline, rect[j], rect[(j+1)%4], Scalar(0, 0, 255), 2, 8);  //绘制最小外接矩形每条边
        }
    }
}
/**
 * @description: 获得能量机关矩形的中心
 * @param {type} box：最小外接矩形集合
 * @return: no
 */
void SivirDetector::getRectangleCenter(vector<RotatedRect> &box){
    cntsmall=0;
    cntbig=0;
    for (int i=0;i<num;i++)
    {
        if (box[i].size.area() > 900)
        {
            big[cntbig++]=box[i].center;
            circle(outline,big[cntbig-1],5,Scalar(0,255,0),-1,8);
        }
        if (box[i].size.area() <900 && box[i].size.area()>400)
        {
            small[cntsmall]=box[i].center;
            box[i].points(allrect[cntsmall]);
            cntsmall++;
            circle(outline,small[cntsmall-1],5,Scalar(255,0,0),-1,8);
        }
    }
    imshow("out",outline);
}
/**
 * @description: 处理筛选后没有发现目标的情况
 * @param {type} no
 * @return: no
 */
void SivirDetector::NoTarget(){
    if (cntsmall != (cntbig/2+1))
    {
        islost=true;
        char tam[100]; 
       // cout<<"No target"<<endl;
	    sprintf(tam, "No target"); 
        putText(src, tam, Point(25, 25), FONT_HERSHEY_SIMPLEX, 1, cvScalar(255,0,255),1);
        return;
    } 
}
/**
 * @description: 只有一个目标下获得能量机关的装甲板，并在用户界面画点
 * @param {type} no
 * @return: no
 */
void SivirDetector::getTargetCoordinateMode1(){
        target.center.x=small[0].x;
        target.center.y=small[0].y;
        Point2f rectl[2];
        int cntl=0;
        Point2f rectr[2];
        int cntr=0;
        for (int j=0;j<4;j++)
        {
            if (allrect[0][j].x<target.center.x)//左侧
            {
                rectl[cntl++]=allrect[0][j];
            }
            if (allrect[0][j].x>target.center.x)//右侧
            {
                rectr[cntr++]=allrect[0][j];
            }
        }
        if (rectl[0].y<rectl[1].y)
        {
            target.rect[0]=rectl[0];//左上
            target.rect[3]=rectl[1];//左下
        }
        else
        {
            target.rect[0]=rectl[1];//左上
            target.rect[3]=rectl[0];//左下
        }
        if (rectr[0].y<rectr[1].y)
        {
            target.rect[1]=rectr[0];//右上
            target.rect[2]=rectr[1];//右下
        }
        else
        {
            target.rect[1]=rectr[1];//右上
            target.rect[2]=rectr[0];//右下
        }
        double d1,d2;
        d1=sqrt((target.rect[0].x-target.rect[3].x)*(target.rect[0].x-target.rect[3].x)+
                (target.rect[0].y-target.rect[3].y)*(target.rect[0].y-target.rect[3].y));
        d2=sqrt((target.rect[0].x-target.rect[1].x)*(target.rect[0].x-target.rect[1].x)+
                (target.rect[0].y-target.rect[1].y)*(target.rect[0].y-target.rect[1].y));
        if (d1>d2)
        {
            Point2f temp;
            temp=target.rect[0];
            target.rect[0]=target.rect[3];
            target.rect[3]=target.rect[2];
            target.rect[2]=target.rect[1];
            target.rect[1]=temp;
        }
        Scalar color4[4]={Scalar(255,0,255),Scalar(255,0,0),Scalar(0,255,0),Scalar(0,255,255)};
        //左上紫色 右上蓝色 右下绿色 左下黄色 
        for (int k=0;k<4;k++)
        {
            circle(src,Point(target.rect[k].x,target.rect[k].y),5,color4[k],-1,8);
        }
}
/**
 * @description: 多目标下获得能量机关装甲板，并在用户界面画点
 * @param {type} no
 * @return: no
 */
void SivirDetector::getTargetCoordinateMode2(int flag[20]){
    for (int i=0;i<cntsmall;i++)
        {
            if (flag[i]==0) 
            {
                target.center.x=small[i].x;
                target.center.y=small[i].y;
                Point2f rectl[2];
                int cntl=0;
                Point2f rectr[2];
                int cntr=0;
                for (int j=0;j<4;j++)
                {
                    if (allrect[i][j].x<target.center.x)//左侧
                    {
                        rectl[cntl++]=allrect[i][j];
                    }
                    if (allrect[i][j].x>target.center.x)//右侧
                    {
                        rectr[cntr++]=allrect[i][j];
                    }
                }
                if (rectl[0].y<rectl[1].y)
                {
                    target.rect[0]=rectl[0];//左上
                    target.rect[3]=rectl[1];//左下
                }
                else
                {
                    target.rect[0]=rectl[1];//左上
                    target.rect[3]=rectl[0];//左下
                }
                if (rectr[0].y<rectr[1].y)
                {
                    target.rect[1]=rectr[0];//右上
                    target.rect[2]=rectr[1];//右下
                }
                else
                {
                    target.rect[1]=rectr[1];//右上
                    target.rect[2]=rectr[0];//右下
                }
                double d1,d2;
                d1=sqrt((target.rect[0].x-target.rect[3].x)*(target.rect[0].x-target.rect[3].x)+
                        (target.rect[0].y-target.rect[3].y)*(target.rect[0].y-target.rect[3].y));
                d2=sqrt((target.rect[0].x-target.rect[1].x)*(target.rect[0].x-target.rect[1].x)+
                        (target.rect[0].y-target.rect[1].y)*(target.rect[0].y-target.rect[1].y));
                if (d1>d2)
                {
                    Point2f temp;
                    temp=target.rect[0];
                    target.rect[0]=target.rect[3];
                    target.rect[3]=target.rect[2];
                    target.rect[2]=target.rect[1];
                    target.rect[1]=temp;
                }
                Scalar color4[4]={Scalar(255,0,255),Scalar(255,0,0),Scalar(0,255,0),Scalar(0,255,255)};
                //左上紫色 右上蓝色 右下绿色 左下黄色 
                for (int k=0;k<4;k++)
                {
                    circle(src,Point(target.rect[k].x,target.rect[k].y),5,color4[k],-1,8);
                }
                break;
            }
        }
}
/**
 * @description: 设置相机内参和畸变系统
 * @param {type} _A_matrix：相机参数
 *               distCoeffs：相机参数
 * @return: no
 */
void SivirDetector::setCamera(Mat  &_A_matrix,Mat &distCoeffs){
     // intrinsic camera parameters
    _A_matrix.at<double>(0, 0) = 1290.9751;            //      [ fx   0  cx ]1059.2770; 
    _A_matrix.at<double>(1, 1) = 1291.04293;            //      [  0  fy  cy ]
    _A_matrix.at<double>(0, 2) = 278.88592;                  //      [  0   0   1 ]
    _A_matrix.at<double>(1, 2) = 266.40793;
    _A_matrix.at<double>(2, 2) = 1;
     // vector of distortion coefficients
    distCoeffs.at<double>(0,0) = -0.49807;
    distCoeffs.at<double>(0,1) = 0.86868;
    distCoeffs.at<double>(0,2) = 0.00142;
    distCoeffs.at<double>(0,3) = -0.00180;
    distCoeffs.at<double>(0,4) = 0;
}
/**
 * @description: 等待获得有价值的图像
 * @param {type} no
 * @return: no
 */
void SivirDetector::waitPreparation(){
    if (islost==true)
    {
        return;
    }
    if (clockwise==0)
    {
        if (prep<5)
        {
            predict[prep].x=target.center.x;
            predict[prep].y=target.center.y;
            prep++;
        }
        if (prep==4)
        {
            float x1,y1,x2,y2,x3,y3;
            x1=predict[0].x;
            y1=predict[0].y;
            x2=predict[2].x;
            y2=predict[2].y;
            x3=predict[4].x;
            y3=predict[4].y;
            float cans=(x2-x1)*(y3-y1)-(y2-y1)*(x3-x1);
            if (cans>0) clockwise=-1;//逆时针
            else clockwise=1;
            prep=0;
        }
        return;
    }
}
/**
 * @description: 获取目标坐标.能量机关默认动（==9）要考虑能量机关移动带来的误差。（==1时为能量机关不动的模式）
 * @param {type} no
 * @return: no
 */
void SivirDetector::attackingStationaryTarget(){
    float dt = 1;   //所有迟到的时间/s
        //旋转变换
        float theta;
        float costheta,sintheta;
        theta=clockwise*3.14159/3*dt;
        costheta=cos(theta);
        sintheta=sin(theta);
        cout<<"costheta:"<<costheta<<endl;
        cout<<"sintheta:"<<sintheta<<endl;
        /*
        x0= (x - rx0)*cos(a) - (y - ry0)*sin(a) + rx0 ;

        y0= (x - rx0)*sin(a) + (y - ry0)*cos(a) + ry0 ;
        */
        float xx,yy;
        xx=target.center.x;
        yy=target.center.y;
        target.center.x=(xx-heart.center.x)*costheta-(yy-heart.center.y)*sintheta+heart.center.x;
        target.center.y=(xx-heart.center.x)*sintheta+(yy-heart.center.y)*costheta+heart.center.y;
        cout<<"xx:"<<xx<<endl;
        cout<<"yy:"<<yy<<endl;
        cout<<"after change x:"<<target.center.x<<endl;
        cout<<"after change y:"<<target.center.y<<endl;
        for (int i=0;i<4;i++)
        {
            xx=target.rect[i].x;
            yy=target.rect[i].y;
            target.rect[i].x=(xx-heart.center.x)*costheta-(yy-heart.center.y)*sintheta+heart.center.x;
            target.rect[i].y=(xx-heart.center.x)*sintheta+(yy-heart.center.y)*costheta+heart.center.y;
        }
        Scalar color4[4]={Scalar(255,0,255),Scalar(255,0,0),Scalar(0,255,0),Scalar(0,255,255)};
        //左上紫色 右上蓝色 右下绿色 左下黄色 
        for (int k=0;k<4;k++)
        {
            circle(src,Point(target.rect[k].x,target.rect[k].y),5,color4[k],-1,8);
        }
}
/**
 * @description: 对应照相机3D空间坐标轴，求相机在世界坐标系中的坐标，求旋转矩阵的转置
 * @param {type} _R_matrix：旋转矩阵
 *               _t_matrix：平移矩阵
 * @return: no
 */
void SivirDetector::calculatingCoordinates(Mat &_R_matrix,Mat &_t_matrix){
    //对应照相机3D空间坐标轴
    //求相机在世界坐标系中的坐标
    //求旋转矩阵的转置
    double m00,m01,m02;
    double m10,m11,m12;
    double m20,m21,m22;
    m00=_R_matrix.at<double>(0,0);
    m01=_R_matrix.at<double>(0,1);
    m02=_R_matrix.at<double>(0,2);
    m10=_R_matrix.at<double>(1,0);
    m11=_R_matrix.at<double>(1,1);
    m12=_R_matrix.at<double>(1,2);
    m20=_R_matrix.at<double>(2,0);
    m21=_R_matrix.at<double>(2,0);
    m22=_R_matrix.at<double>(2,2);
    double x=0.0,y=0.0,z=0.0;
    //先减去平移矩阵
    double tempx=0.0,tempy=0.0,tempz=0.0;
    tempx=0-_t_matrix.at<double>(0,0);
    tempy=0-_t_matrix.at<double>(1,0);
    tempz=0-_t_matrix.at<double>(2,0);
    //乘以矩阵的逆
    x=m00*tempx+m10*tempy+m20*tempz;
    y=m01*tempx+m11*tempy+m21*tempz;
    z=m02*tempx+m12*tempy+m22*tempz;
    char tam1[100]; 
	sprintf(tam1, "cam in world(%0.0f,%0.0f,%0.0f)",x,y,z); 
    putText(src, tam1, Point(15, 15), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(0,0,255),1);
    //求装甲板中心在相机坐标系中的坐标
    double newx=0.0,newy=0.0,newz=0.0;
    double drop=0.0;
    drop=5*0.07;
    newx=m00*65+m01*27.5+m02*0+_t_matrix.at<double>(0,0);
    newy=m10*65+m11*27.5+m12*0+_t_matrix.at<double>(1,0)-40-drop*1000;
    newz=m20*65+m21*27.5+m22*0+_t_matrix.at<double>(2,0);
    char tam2[100]; 
	sprintf(tam2, "center in cam(%0.0f,%0.0f,%0.0f)",newx,newy,newz); 
    putText(src, tam2, Point(15, 30), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(0,0,255),1);
    //算欧拉角
    //pitch：绕x轴  roll：绕z轴 yaw：绕y轴
    float pitch,roll,yaw;
    double vec[3];
    vec[0]=newx;
    vec[1]=newy;
    vec[2]=newz;
    yaw=atan(vec[0]/vec[2])*180/PI;
    pitch=atan(vec[1]/vec[2])*180/PI;
    if(abs(yaw)>0.02) pnpresult.yaw=yaw;
    else pnpresult.yaw=0;
    if(abs(pitch)>0.02) pnpresult.pitch=pitch;
    else pnpresult.pitch=0;
    if (islost==true)
    {
        pnpresult.yaw=0;
        pnpresult.pitch=0;
    }
    char tam3[100]; 
	sprintf(tam3, "tan yaw=%0.4f   tan pich=%0.4f",vec[0]/vec[2],vec[1]/vec[2]); 
    putText(src, tam3, Point(15, 45), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(0,0,255),1);
    char tam4[100]; 
	sprintf(tam4, "yaw=%0.4f   pich=%0.4f",pnpresult.yaw,pnpresult.pitch); 
    putText(src, tam4, Point(15, 60), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(0,0,255),1);
}
