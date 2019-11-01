/*
 * @Copyright: CS of BIT
 * @Author: 孟文珩
 * @File name: 
 * @Version: 
 * @Date: 2019-10-26 18:01:37
 * @LastEditTime: 2019-11-01 19:38:20
 * @LastEditors: 
 * @Description: 
 */
#include "InfantryArmorDetector.h"

#ifdef INFANTRY

#define PI 3.14159265
int valu[6] = {1,2,3,4,5,6};

//********************************************运行主体函数**********************************************//
void InfantryArmorDetector::getCenter(Mat &source_img, TargetData &armor_data)
{
    src = source_img;
    if (!roiimg.empty())
        imshow("roi",roiimg);
    getBinaryImage();
    //imshow("bin",binary);
    getContours();
    getTarget();
    getPnP();
}

/**
 * @Author: YY
 * @Description: 将图像二值化为灰度图，并进行膨胀处理
 * @Param: none
 * @Return: void
 * @Throw: 
 */
//二值化
void InfantryArmorDetector::getBinaryImage()
{
    Mat gry;
    src.copyTo(gry);
    vector<Mat> imgChannels;
    //分离三通道，BGR,0--B,2--R
    split(src,imgChannels);
    //红色处理
    if(this->isred)  
    {
        gry=imgChannels.at(2)-imgChannels.at(0);
    }
    //蓝色处理
    else    
    {
        gry = imgChannels.at(0) - imgChannels.at(2);
    }
    //原图遍历，将roi区域外的图像矩阵置0，类似裁剪，减少threshold运行时间
    for (int row=0;row<src.rows;row++)
    {
        for (int col=0;col<src.cols;col++)
        {
            if (row<=roi.lefttop.y || row>=roi.lefttop.y+roi.rheight || col<=roi.lefttop.x || col>=roi.lefttop.x+roi.rwidth) 
            {
                gry.at<uchar>(row,col)=0;
                continue;
            }
        }
    }
    
    threshold(gry,binary,100,255,CV_THRESH_BINARY);
    //获取自定义核，这里使用半径为三的圆进行膨胀
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3)); //第一个参数MORPH_RECT表示矩形的卷积核，当然还可以选择椭圆形的、交叉型的
    //膨胀操作
    dilate(binary, binary, element);
    //erode(binary, binary, element);
}

void InfantryArmorDetector::getContours()
{
    vector<Vec4i> hierarcy;
    Point2f rect[4];
    src.copyTo(outline);
    findContours(binary, contours, hierarcy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    num=contours.size();//轮廓的数量
    vector<Rect> boundRect(contours.size());//最小外接正矩形的左上角点与矩形长宽集合
    vector<RotatedRect> box(contours.size());//最小外接矩形集合
    for (int i=0;i<num;i++)
    {
        //计算每个轮廓的最小外接斜矩形
        box[i]=minAreaRect(Mat(contours[i]));
        //获得最小外接正矩形的左上角点坐标以及矩形的长宽
        boundRect[i]=boundingRect(Mat(contours[i]));
        //在矩形中心绘制一个圆
        circle(outline,Point(box[i].center.x,box[i].center.y),2,Scalar(0,255,0),-1,8);
        box[i].points(rect);
        //利用矩形对角线两点绘制一个矩形，其中左上角点的坐标加上该矩形的长宽即为该矩形的另一对角点
        rectangle(outline, Point(boundRect[i].x, boundRect[i].y), Point(boundRect[i].x + boundRect[i].width, boundRect[i].y + boundRect[i].height), Scalar(0, 255, 0), 1, 8);
         for(int j=0; j<4; j++)
         {
             //绘制最小外接斜矩形每条边
            line(outline, rect[j], rect[(j+1)%4], Scalar(0, 0, 255), 1, 8); 
         }
    }
    for (int i=0;i<num;i++)
    {
        //输出最小外接斜矩形的角度和长宽
        cout<<"num "<<i<<": area="<<box[i].size.area()<<" angle="<<box[i].angle<<" a="<<box[i].size.height<<" b="<<box[i].size.width<<endl;
    }
    memset(matchrank,0,sizeof(matchrank));
    for (int i=0;i<num;i++)
    {
        for (int j=i+1;j<num;j++)
        {   
            //部分参数赋值，为了简化后续子判断函数传参
            //角度
            anglei=box[i].angle;
            anglej=box[j].angle;
            //面积
            areai=box[i].size.area() > box[j].size.area() ? box[i].size.area() : box[j].size.area();
            areaj=box[i].size.area() < box[j].size.area() ? box[i].size.area() : box[j].size.area();
            //矩形中心连线长
            double d=sqrt((box[i].center.x-box[j].center.x)*(box[i].center.x-box[j].center.x)+(box[i].center.y-box[j].center.y)*(box[i].center.y-box[j].center.y));
            //数组位置
            local_num_i = i;
            local_num_j = j;
            
            //去掉太斜的矩形
            inclinationAngleFilter(box);
            //根据长宽筛选
            aspectRadioFilter(box);
            if (mode==3)
            {
                //相对位置筛选           
                relativePositionFilter(box,mode_3);
                //根据相对角度筛选
                relativeTiltAngleFilter(mode_3);
                //面积比
                areaRadioFilter(mode_3);
                //连线长
                //两个矩形中心的连线长度
                if(d >= longi * 4.7 || d >= longj * 4.7 || d<1.5*longi || d<1.5*longi) matchrank[i][j] -= 10000;
                else if (d <= longi * 4.7 && d <= longj * 4.7 && d>1.5*longi && d>1.5*longi) matchrank[i][j] += 100;
                if (d >= longi * 3 || d >= longj * 3) dis[i][j]=0;//=1为小装甲
                else dis[i][j]=1;
                cout<<"i j d:"<<i<<" "<<j<<" "<<d<<endl;
            }
            else if (mode==1)
            {
                armor_data.is_big = false;
                //相对位置筛选           
                relativePositionFilter(box,mode_1);
                //根据角度筛选
                relativeTiltAngleFilter(mode_1);
                //面积比
                areaRadioFilter(mode_1);
                //中心连线长筛选
                if (d >= longi * 7|| d >= longj * 7 || d<1.5*longi || d<1.5*longj) matchrank[i][j] -= 10000;
                else if (d >= longi * 2.7 || d >= longj * 2.7 || d<1.5*longi || d<1.5*longi) matchrank[i][j] -= 10000;
                else if (d <= longi * 2.7 && d <= longj * 2.7 && d>1.5*longi && d>1.5*longi) matchrank[i][j] += 100;
                cout<<"i j d:"<<i<<" "<<j<<" "<<d<<endl;
            }
            else
            {
                armor_data.is_big = true;
                //相对位置筛选           
                relativePositionFilter(box,mode_2);
                //根据角度筛选
                relativeTiltAngleFilter(mode_2);
                //面积比
                areaRadioFilter(mode_2);
                //连线长
                if (d >= longi * 7|| d >= longj * 7 || d<1.5*longi || d<1.5*longj) matchrank[i][j] -= 10000;
                else if (d >= longi * 5 || d >= longj * 5 || d<3*longi || d<3*longi) matchrank[i][j] -= 10000;
                else if (d <= longi * 5 && d <= longj * 5 && d>3*longi && d>3*longi) matchrank[i][j] += 100;
                cout<<"i j d:"<<i<<" "<<j<<" "<<d<<endl;
            }
        }
    }                                                                                 
}

void InfantryArmorDetector::getTarget()
{
    //初始化
    maxpoint=-8000;
    besti=-1;
    bestj=-1;
    //遍历，得到最优组合并记录位置
    for (int i=0;i<num;i++)
        for (int j=i+1;j<num;j++)
        {
            local_num_i = i;
            local_num_j = j;
            // cout<<"matchrank "<<i<<" "<<j<<" :"<<matchrank[i][j]<<endl;
            // if (maxpoint<matchrank[i][j])
            // {
            //     maxpoint=matchrank[i][j];
            //     if (mode == 3) 
            //     {
            //         //cout<<"判断为大"<<endl;
            //         if (dis[i][j]==1) armor_data.is_big = false;
            //         else armor_data.is_big = true;
            //     }
            //     else;
            //     besti=i;
            //     bestj=j;
            // }
            pickBest();
        }
    //视野中没有装甲板
    if (besti==-1 || bestj==-1) 
    {
        // armor_data.is_get = false;
        // if (roi.lefttop.x - 10<=0) roi.lefttop.x=0;
        // else roi.lefttop.x -= 10;
        // if (roi.lefttop.y - 10<=0) roi.lefttop.y=0;
        // else roi.lefttop.y -= 10;
        // if (roi.lefttop.x + roi.rwidth + 20 > src.cols) roi.rwidth=src.cols-roi.lefttop.x;
        // else roi.rwidth=roi.rwidth + 20;
        // if (roi.lefttop.y + roi.rheight + 20 > src.rows) roi.rheight=src.rows-roi.lefttop.y;
        // else roi.rheight=roi.rheight + 20;
        // roiimg=src(Rect(roi.lefttop.x,roi.lefttop.y,roi.rwidth,roi.rheight));
        // return;
        lostArmor();
        return;
    }

    armor_data.is_get = true;
    boxi=minAreaRect(Mat(contours[besti]));
    boxj=minAreaRect(Mat(contours[bestj]));
    target.center=Point2f((boxi.center.x+boxj.center.x)/2,(boxi.center.y+boxj.center.y)/2);
    //cout<<"i "<<besti<<" :x="<<boxi.center.x<<" y="<<boxi.center.y<<endl;
    //cout<<"j "<<bestj<<" :x="<<boxj.center.x<<" y="<<boxj.center.y<<endl;
    //cout<<"target : x="<<target.center.x<<" y="<<target.center.y<<endl;
    circle(src,Point(target.center.x,target.center.y),5,Scalar(255,0,0),-1,8);
    circle(outline,Point(target.center.x,target.center.y),5,Scalar(255,0,0),-1,8);
    char tam[100]; 
	//sprintf(tam, "(%0.0f,%0.0f)",target.center.x,target.center.y);
    //在图片左上角输出计算后的信息
    putText(src, tam, Point(target.center.x, target.center.y), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(255,0,255),1);
    //获取周围四个点的坐标
    if (boxi.center.x > boxj.center.x)
    {
        RotatedRect temp;
        temp=boxi;
        boxi=boxj;
        boxj=temp;
    }
    Point2f rect1[4];
    Point2f rect2[4];
    Point2f rect3[4]={Point2f(0,0)};
    boxi.points(rect1);
    boxj.points(rect2);
    for (int i=0;i<4;i++)
    {
        if (rect1[i].y<boxi.center.y) 
        {
            rect3[0].x+=rect1[i].x;   //左上
            rect3[0].y+=rect1[i].y;
        }
        if (rect1[i].y>boxi.center.y) 
        {
            rect3[1].x+=rect1[i].x;   //左下
            rect3[1].y+=rect1[i].y;
        }
    }
    for (int i=0;i<4;i++)
    {
        if (rect2[i].y<boxj.center.y) 
        {
            rect3[2].x+=rect2[i].x;   //右上
            rect3[2].y+=rect2[i].y;
        }
        if (rect2[i].y>boxj.center.y) 
        {
            rect3[3].x+=rect2[i].x;   //右下
            rect3[3].y+=rect2[i].y;
        }
    }
    Scalar color4[4]={Scalar(255,0,255),Scalar(255,0,0),Scalar(0,255,0),Scalar(0,255,255)};
    //左上紫色 左下蓝色 右上绿色 右下黄色 
    for (int i=0;i<4;i++)
    {
        target.rect[i]=Point2f(rect3[i].x/2,rect3[i].y/2);
        circle(src,Point(target.rect[i].x,target.rect[i].y),3,color4[i],-1,8);
       // sprintf(tam, "(%0.0f,%0.0f)",target.rect[i].x,target.rect[i].y); 
        putText(src, tam, Point(target.rect[i].x, target.rect[i].y), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(255,0,255),1);
    }
    float x,y;
    float roif=0.0;
    if (mode ==2) roif = 30.0;
    else roif=20.0;
    if (target.rect[0].x-roif<0) x=0; else x=target.rect[0].x-roif;
    if (target.rect[0].y-roif<0) y=0; else y=target.rect[0].y-roif;
    roi.lefttop=Point2f(x,y);
    int h,w;
    w=target.rect[2].x-target.rect[0].x+2*roif;
    h=target.rect[3].y-target.rect[0].y+2*roif;
    //cout<<"w h:"<<w<<" "<<h<<endl;
    if (roi.lefttop.x+w>src.cols) roi.rwidth=src.cols-roi.lefttop.x;
    else roi.rwidth=w;
    if (roi.lefttop.y+h>src.rows) roi.rheight=src.rows-roi.lefttop.y;
    else roi.rheight=h;
    roiimg=src(Rect(roi.lefttop.x,roi.lefttop.y,roi.rwidth,roi.rheight));
}

void InfantryArmorDetector::getPnP()
{
    //控制点在世界坐标系中
    //按照顺时针圧入，左上是第一个点
    vector<Point3f> objP;
    Mat objM;
    if (armor_data.is_big){
        objP.clear();
        objP.push_back(Point3f(0,0,0));
        objP.push_back(Point3f(225,0,0));
        objP.push_back(Point3f(225,55,0));
        objP.push_back(Point3f(0,55,0));
        Mat(objP).convertTo(objM,CV_32F);
    }
    else{
        objP.clear();
        objP.push_back(Point3f(0,0,0));
        objP.push_back(Point3f(130,0,0));
        objP.push_back(Point3f(130,55,0));
        objP.push_back(Point3f(0,55,0));
        Mat(objP).convertTo(objM,CV_32F);
    }
   
    //目标四个点按照顺时针圧入，左上是第一个点
    vector<Point2f> points;
    for (int i=0;i<4;i++)
    {
        target.rect[i].x+=320;
        target.rect[i].y+=320;
    }
    points.clear();
    points.push_back(target.rect[0]);
    points.push_back(target.rect[2]);
    points.push_back(target.rect[3]);
    points.push_back(target.rect[1]);
    //设置相机内参和畸变系统
    Mat  _A_matrix = cv::Mat::zeros(3, 3, CV_64FC1);   // intrinsic camera parameters
    _A_matrix.at<double>(0, 0) = 1059.2770;            //      [ fx   0  cx ]1059.2770; 
    _A_matrix.at<double>(1, 1) = 1059.2770;            //      [  0  fy  cy ]
    _A_matrix.at<double>(0, 2) = 640;                  //      [  0   0   1 ]
    _A_matrix.at<double>(1, 2) = 512;
    _A_matrix.at<double>(2, 2) = 1;
    Mat distCoeffs = cv::Mat::zeros(4, 1, CV_64FC1); // vector of distortion coefficients
    distCoeffs.at<double>(0,0) = 0.137406;
    distCoeffs.at<double>(0,1) = -0.227949;
    distCoeffs.at<double>(0,4) = 0.115991;
    //设置旋转、平移矩阵，旋转、平移向量
    Mat _R_matrix = cv::Mat::zeros(3, 3, CV_64FC1);   // rotation matrix
    Mat _t_matrix = cv::Mat::zeros(3, 1, CV_64FC1);   // translation matrix
    Mat rvec = cv::Mat::zeros(3, 1, CV_64FC1);          // output rotation vector
    Mat tvec = cv::Mat::zeros(3, 1, CV_64FC1);          // output translation vector
    solvePnP(objP,points,_A_matrix,distCoeffs,rvec,tvec);
    Rodrigues(rvec,_R_matrix);                   // converts Rotation Vector to Matrix
    _t_matrix = tvec;                            // set translation matrix
    //对应照相机3D空间坐标轴
    /*
    vector<Point3f> reference_objP;
    vector<Point2f> reference_imgP;
    reference_objP.push_back(Point3f(65,27.5,0.0));//原点
    reference_objP.push_back(Point3f(90,27.5,0.0));//x轴
    reference_objP.push_back(Point3f(65,52.5,0.0));//y轴
    reference_objP.push_back(Point3f(65,27.5,25));//z轴
    projectPoints(reference_objP,rvec,tvec,_A_matrix,distCoeffs,reference_imgP);
    line(outline,reference_imgP[0],reference_imgP[1],Scalar(0,0,255),2);//红色X轴   
    line(outline,reference_imgP[0],reference_imgP[2],Scalar(0,255,0),2);//绿色Y轴
    line(outline,reference_imgP[0],reference_imgP[3],Scalar(255,0,0),2);//蓝色Z轴
    */
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
    if (mode==1 || mode ==3)
    {
        if (armor_data.is_big){
            newx=m00*115+m01*27.5+m02*0+_t_matrix.at<double>(0,0)-20;//加往右
            newz=m20*115+m21*27.5+m22*0+_t_matrix.at<double>(2,0)+25;
            newx = newx - newz/2000*15;
            drop=5*(newz / 1000 / 26)*(newz / 1000 / 26);
            newy=m10*113+m11*27.5+m12*0+_t_matrix.at<double>(1,0)-60-drop*1000;;//减往上-newz*newz/20000 -0.026x+88
        }
        else
        {
            newx=m00*65+m01*27.5+m02*0+_t_matrix.at<double>(0,0)-20;//加往右
            newz=m20*65+m21*27.5+m22*0+_t_matrix.at<double>(2,0)+25;
            newx = newx - newz/2000*15;
            drop=5*(newz / 1000 / 26)*(newz / 1000 / 26);
            newy=m10*65+m11*27.5+m12*0+_t_matrix.at<double>(1,0)-60-drop*1000;//减往上-newz*newz/20000 -0.026x+88
        }
    }
    else
    {
        newx=m00*115+m01*27.5+m02*0+_t_matrix.at<double>(0,0)-20;//加往右
        newz=m20*115+m21*27.5+m22*0+_t_matrix.at<double>(2,0)+25;
        newx = newx - newz/2000*15;
        newy=m10*113+m11*27.5+m12*0+_t_matrix.at<double>(1,0)-60-50;//;//减往上-newz*newz/20000 -0.026x+88
    }
    armor_data.atocDistance = newz;
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
    if(abs(yaw)>0.02) armor_data.yaw_angle = yaw;
    else armor_data.yaw_angle = 0;
    if(abs(pitch)>0.02) armor_data.pitch_angle = pitch;
    else armor_data.pitch_angle = 0;
    if (armor_data.is_get == false)
    {
        armor_data.yaw_angle = 0;
        armor_data.pitch_angle = 0;
        armor_data.atocDistance = 0;
    }
    //char tam3[100]; 
	//sprintf(tam3, "tan yaw=%0.4f   tan pich=%0.4f",vec[0]/vec[2],vec[1]/vec[2]); 
    //putText(src, tam3, Point(15, 45), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(0,0,255),1);
    char tam4[100]; 
	sprintf(tam4, "yaw=%0.4f   pich=%0.4f",armor_data.yaw_angle, armor_data.pitch_angle); 
    putText(src, tam4, Point(15, 60), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(0,0,255),1);
    if (armor_data.is_big){
        putText(src,"BIG ARMOR",Point(15, 75), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(0,255,0),1);
    }
    else
    {
        putText(src,"SMALL ARMOR",Point(15, 75), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(0,255,0),1);
    }
    if (isred){
        putText(src,"RED ARMOR",Point(15, 90), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(0,0,255),1);
    }
    else
    {
        putText(src,"BLUE ARMOR",Point(15, 90), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(255,0,0),1);
    }
}
//********************************************其他功能性函数********************************************//

InfantryArmorDetector::InfantryArmorDetector()
{
    armor_data.is_get = false;
    isred=true;
    mode=3;
    roi.lefttop=Point2f(0,0);
    roi.rwidth=src.cols;
    roi.rheight=src.rows;
    roi.level=1;
    roiimg=src(Rect(roi.lefttop.x,roi.lefttop.y,roi.rwidth,roi.rheight));
}

InfantryArmorDetector::InfantryArmorDetector(Mat &src)
{
    armor_data.is_get = false;
    isred=true;
    mode=3;
    roi.lefttop=Point2f(0,0);
    roi.rwidth=src.cols;
    roi.rheight=src.rows;
    roi.level=1;
    roiimg=src(Rect(roi.lefttop.x,roi.lefttop.y,roi.rwidth,roi.rheight));
}

/**
 * @Author: 孟文珩
 * @Description: 用于观察各部分函数作用，以及调参。可同时显示6乃至更多张图片以便对比。
 * @Param: src 原图
 * @Param: armor_data 保存了筛选后得到的装甲板的所有数据，详情参见TargetData
 * @Return: void
 * @Throw: 
 */
void InfantryArmorDetector::debugGetCenter(Mat &src, TargetData &armor_data)
{
    for(int i = 0;i < 6;i++)
    {
        if (!roiimg.empty())
        imshow("roi",roiimg);
        getBinaryImage();
        imshow("bin"+std::to_string(i),binary);
        getContours();
        getTarget();
        getPnP();
        imshow("out",outline);
        imshow("last"+std::to_string(i),src);
        waitKey(1000);  //延时1s
    }
}

void InfantryArmorDetector::inclinationAngleFilter(vector<RotatedRect>& box)
{
    if ((box[local_num_i].size.width > box[local_num_i].size.height && box[local_num_i].angle>-70) || (box[local_num_i].size.width < box[local_num_i].size.height && box[local_num_i].angle<-20)) matchrank[local_num_i][local_num_j]-=10000;
    if ((box[local_num_j].size.width > box[local_num_j].size.height && box[local_num_j].angle>-70) || (box[local_num_j].size.width < box[local_num_j].size.height && box[local_num_j].angle<-20)) matchrank[local_num_i][local_num_j]-=10000;
}
double InfantryArmorDetector::getLongSide(double side_1,double side_2)
{
    return side_1 > side_2 ? side_1 : side_2;
}
double InfantryArmorDetector::getShortSide(double side_1,double side_2)
{
    return side_1 < side_2 ? side_1 : side_2;
}
void InfantryArmorDetector::aspectRadioFilter(vector<RotatedRect>& box)
{
    //i排序
    longi=getLongSide(box[local_num_i].size.height, box[local_num_i].size.width);
    shorti=getShortSide(box[local_num_i].size.height, box[local_num_i].size.width);
    //j排序
    longj=getLongSide(box[local_num_j].size.height, box[local_num_j].size.width);
    shortj=getShortSide(box[local_num_j].size.height, box[local_num_j].size.width);
    //长宽比判断筛选
    if ((longi/shorti) <= 1.3 || (longj/shortj) <= 1.3) matchrank[local_num_i][local_num_j]-=10000;
    else if ((longi/shorti) >= 9.5 || (longj/shortj) >= 9.5) matchrank[local_num_i][local_num_j]-=10000;
    else if ((longi/shorti)>=4 && (longi/shorti)<=7 && (longj/shortj)>=4 && (longj/shortj)<=7)matchrank[local_num_i][local_num_j]+=100;
    else if ((longi/shorti)>=3.5 && (longi/shorti)<=7.5 && (longj/shortj)>=3.5 && (longj/shortj)<=7.5) matchrank[local_num_i][local_num_j]+=80;
}
void InfantryArmorDetector::relativePositionFilter(vector<RotatedRect>& box,ModeParam mode)
{
    if ((box[local_num_i].center.y-box[local_num_j].center.y)> mode.relative_position_radio *longi || (box[local_num_i].center.y-box[local_num_j].center.y)> mode.relative_position_radio *longj) matchrank[local_num_i][local_num_j]-=10000;
    else if (abs(box[local_num_i].center.y-box[local_num_j].center.y)<3) matchrank[local_num_i][local_num_j] +=100;
    else if (abs(box[local_num_i].center.y-box[local_num_j].center.y)<5) matchrank[local_num_i][local_num_j] +=50;
    else if (abs(box[local_num_i].center.y-box[local_num_j].center.y)<8) matchrank[local_num_i][local_num_j] +=10;
}
void InfantryArmorDetector::relativeTiltAngleFilter(ModeParam mode)
{
    if (abs(anglei-anglej)<=mode.relative_angle[0][0] || abs(anglei-anglej)>=mode.relative_angle[0][1] ) matchrank[local_num_i][local_num_j]+=200;
    else if (abs(anglei-anglej)<=mode.relative_angle[1][0] || abs(anglei-anglej)>=mode.relative_angle[1][1] ) matchrank[local_num_i][local_num_j]+=100;
    else if (abs(anglei-anglej)<=mode.relative_angle[2][0] || abs(anglei-anglej)>=mode.relative_angle[2][1] ) matchrank[local_num_i][local_num_j]+=50;
    else if (abs(anglei-anglej)<=mode.relative_angle[3][0] || abs(anglei-anglej)>=mode.relative_angle[3][1] ) matchrank[local_num_i][local_num_j]+=5;
    else matchrank[local_num_i][local_num_j]-=10000;
}
void InfantryArmorDetector::areaRadioFilter(ModeParam mode)
{
    if (areai< mode.relative_area[0] || areaj< mode.relative_area[0]) matchrank[local_num_i][local_num_j] -= 20000;
    else if (areai/areaj>= mode.relative_area[1]) matchrank[local_num_i][local_num_j] -= 10000;
    else if (areai/areaj>= mode.relative_area[2]) matchrank[local_num_i][local_num_j] -= 100;
    else if (areai/areaj> mode.relative_area[3]) matchrank[local_num_i][local_num_j] += 10;
    else if (areai/areaj> mode.relative_area[4]) matchrank[local_num_i][local_num_j] += 50;
    else  matchrank[local_num_i][local_num_j] += 100;
}
void InfantryArmorDetector::centerConnectionLengthFileterofMode_3(int i,int j)
{
    if(d >= longi * 4.7 || d >= longj * 4.7 || d<1.5*longi || d<1.5*longi) matchrank[i][j] -= 10000;
    else if (d <= longi * 4.7 && d <= longj * 4.7 && d>1.5*longi && d>1.5*longi) matchrank[i][j] += 100;
    if (d >= longi * 3 || d >= longj * 3) dis[i][j]=0;//=1为小装甲
    else dis[i][j]=1;
    // cout <<"i"<<i<<" —— "<<local_num_i<<endl;
    // cout <<"j"<<j<<" —— "<<local_num_j<<endl;
    // cout << "mode_3.center_distance[0] : " << mode_3.center_distance[0][0] << "  " << mode_3.center_distance[0][1] << endl;
    // cout << "mode_3.center_distance[1] : " << mode_3.center_distance[1][0] << "  " << mode_3.center_distance[1][1] << endl;
}
void InfantryArmorDetector::centerConnectionLengthFileter(ModeParam mode)
{
    if (d >= longi * mode.center_distance[0][0]|| d >= longj * mode.center_distance[0][0] || d<longi * mode.center_distance[0][1] || d<longj * mode.center_distance[0][1]) matchrank[local_num_i][local_num_j] -= 10000;
    else if (d >= longi * mode.center_distance[1][0] || d >= longj * mode.center_distance[1][0] || d<mode.center_distance[1][1]*longi || d<mode.center_distance[1][1]*longj) matchrank[local_num_i][local_num_j] -= 10000;
    else if (d <= longi * mode.center_distance[1][0] && d <= longj * mode.center_distance[1][0] && d>mode.center_distance[1][1]*longi && d>mode.center_distance[1][1]*longj) matchrank[local_num_i][local_num_j] += 100;
    cout<<"i j d:"<<local_num_j<<" "<<local_num_j<<" "<<d<<endl;
}

void InfantryArmorDetector::pickBest()
{
    cout<<"matchrank "<<local_num_i<<" "<<local_num_j<<" :"<< matchrank[local_num_i][local_num_j]<<endl;
    if (maxpoint<matchrank[local_num_i][local_num_j])
    {
        maxpoint=matchrank[local_num_i][local_num_j];
        if (mode == 3) 
        {
            if (dis[local_num_i][local_num_j]==1) armor_data.is_big = false;
            else armor_data.is_big = true;
        }
        else;
        besti = local_num_i;
        bestj = local_num_j;
    }
}
void InfantryArmorDetector::lostArmor()
{
    armor_data.is_get = false;
    if (roi.lefttop.x - 10<=0) roi.lefttop.x=0;
    else roi.lefttop.x -= 10;
    if (roi.lefttop.y - 10<=0) roi.lefttop.y=0;
    else roi.lefttop.y -= 10;
    if (roi.lefttop.x + roi.rwidth + 20 > src.cols) roi.rwidth=src.cols-roi.lefttop.x;
    else roi.rwidth=roi.rwidth + 20;
    if (roi.lefttop.y + roi.rheight + 20 > src.rows) roi.rheight=src.rows-roi.lefttop.y;
    else roi.rheight=roi.rheight + 20;
    roiimg=src(Rect(roi.lefttop.x,roi.lefttop.y,roi.rwidth,roi.rheight));
}

#endif