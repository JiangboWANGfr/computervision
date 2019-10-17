/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 13:14:07 +0800
 * @LastEditTime: 2019-10-17 18:31:10 +0800
 * @LastEditors: 
 * @Description: 
 */
#include "InfantryPictureManipulator.h"
#include "SivirDetector.h"

int mode=3;
int findmode=1;
int getcaprsc=0;

VideoCapture cap;

InfantryPictureManipulator::InfantryPictureManipulator(string serial_port_device,
                                                   string path,
                                                   double fps,
                                                   int width_video_size,
                                                   int height_video_size)
    : PictureManipulator(serial_port_device,
                         path,
                         fps,
                         width_video_size,
                         height_video_size)
{
}

InfantryPictureManipulator::InfantryPictureManipulator()
{
}

InfantryPictureManipulator::~InfantryPictureManipulator()
{
}

int InfantryPictureManipulator::manipulatePicture(Mat source_image_directly_from_camera)
{
    Mat img1 = source_image_directly_from_camera;
    cap.open("/dev/video0");
    DetermineRecognitionMode();

        if ( findmode == ARMOR_MODE )
        {
           IdentifyAramorMode(img1);
           //videoWrtLYY<<frame;
        }
        else if (findmode==2)
        {
           IdentifyMode2(img1);
           //videoWrtLYY<<frame;
        }

    waitKey(1);
}

//-------------------------------------------------
/**
\brief 判断识别模式
\return void
*/
//-------------------------------------------------

void InfantryPictureManipulator::DetermineRecognitionMode(){
    stm32.readMode(mode);
    if (mode == 1)
    {
        findmode=1;
        armor_detector.mode = 1;//识别小装甲
    }
    if (mode == 2)
    {
        findmode=1;
        armor_detector.mode = 2;//识别大装甲
    } 
    if (mode == 3)//大小都识别
    {
        findmode=1;
        armor_detector.mode = 3;
    }
    if (mode == 4)//静止能量机关
    {
        findmode=2;
        Sivir.mode=1;
    }
    if (mode == 5)//转动
    {
        findmode=2;
        Sivir.mode=2;
    }
    /*if (mode == 7)//对方是红色
    {
        detector.isred = true;
    }
    if (mode == 8)
    {
        detector.isred = false;
    }*/
    armor_detector.isred = false;
    //detector.mode = 3;
    std::cout<<"mode:"<<mode<<endl;
    std::cout<<"detector.mode:"<<armor_detector.mode<<endl;
}

//-------------------------------------------------
/**
\brief 寻找模式：ARAMO_RMODE
\return void
*/
//-------------------------------------------------

void InfantryPictureManipulator::IdentifyAramorMode(Mat &frame){
    if (getcaprsc==1)//相机切换
        {
            getcaprsc=0;
            cap.release();
            std::cout<<"now mode:"<<mode<<endl;
        }
        //videoWrtL << frame;
        armor_detector.getCenters(frame, armor_data);
        // circle(frame,armor_detector.target.center,8,Scalar(255,0,0),2,8,0);
        //videoWrtLYY << frame;


        IdentifyTarget();
}

//-------------------------------------------------
/**
\brief 寻找模式：2
\return void
*/
//-------------------------------------------------

void InfantryPictureManipulator::IdentifyMode2(Mat &frame){
    if (getcaprsc==0)//相机切换
        {
            getcaprsc=1;
            std::cout<<"now mode:"<<mode<<endl;
            cap.open("/dev/video0");
        }
        cap >> frame;
        //frame=srcframe.clone();
        resize(frame,frame,Size(640,480));
        //videoWrtL << frame;
        Sivir.getResult(frame);
        circle(frame,Sivir.target.center,8,Scalar(255,0,0),2,8,0); 
        //videoWrtLYY << frame;         
        //videoWrtLYY<<Sivir.src;

        GetAramor();
}

//-------------------------------------------------
/**
\brief 成功识别到装甲板
\return void
*/
//-------------------------------------------------

void InfantryPictureManipulator::GetAramor(){
    if (Sivir.islost==false) std::cout<<"find the armor successfully!"<<endl;
        float yaw,pitch;
        yaw=Sivir.pnpresult.yaw;
        pitch=Sivir.pnpresult.pitch;
        std::cout<<"yaw:"<<yaw<<"  pitch:"<<pitch<<endl;
        stm32.sendAngle(yaw,pitch);
        //double afterfind=clock();
        //cout<<"find sivir time:="<<(afterfind-now)/CLOCKS_PER_SEC * 1000<<"ms"<<endl;
}

//-------------------------------------------------
/**
\brief 识别目标
\return void
*/
//-------------------------------------------------

void InfantryPictureManipulator::IdentifyTarget(){
    
    int delay;
    float yaw,pitch,dist;
    float flag;
    if (armor_detector.islost == false)
    {
        // yaw=armor_detector.pnpresult.yaw;
        // pitch=armor_detector.pnpresult.pitch;
        // dist=armor_detector.pnpresult.dist/1000;
        //cout<<"yaw:"<<yaw<<"  pitch:"<<pitch<<" dist:"<<dist<<endl;
        flag=1.0;
        delay=0;
    }
    else
    {
        if (delay<5)
        {
            yaw =  yaw;
            pitch = pitch;
            dist = dist;
            flag = 1.0;
            delay++;
        }
        else
        {
            // yaw=armor_detector.pnpresult.yaw;
            // pitch=armor_detector.pnpresult.pitch;
            // dist=armor_detector.pnpresult.dist/1000;
            //cout<<"yaw:"<<yaw<<"  pitch:"<<pitch<<" dist:"<<dist<<endl;
            flag=0.0;
        }
    }
    stm32.sendAngleDist(yaw,pitch,dist,flag);
    cout<<"flag:"<<flag<<endl;
    //double afterfind=clock();
    //cout<<"find armor time:="<<(afterfind-now)/CLOCKS_PER_SEC * 1000<<"ms"<<endl;
}
