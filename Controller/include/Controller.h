/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 14:23:07 +0800
 * @LastEditTime: 2019-10-26 18:30:05 +0800
 * @LastEditors: 
 * @Description: 改文件只允许main进行引用
 */
#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "header.h"
#include "actions.h"
#include "PictureManipulator.h"
#include "Camera.h"
#include "SerialPort.hpp"
#include "Socket.h"

class Controller
{
private:
    static int num_of_controller;//controller句柄个数
    static pthread_mutex_t s_mutex;//防止多线程同时创建controller
    int controller_handle;//controller句柄
    double fps;
    Size video_size;
    string filename;
    int width_video_size, height_video_size; //用系统时间取名，防止同名文件自我覆盖
    VideoWriter src_video, fin_video;
    ofstream filterData; // 记录装甲数据输出为csv文件，方便建模分析

    SerialPort stm32;
    Socket client;

    bool is_get_image = true;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    int is_ready_to_manipulate = 0;
    Mat source_img;
    Mat img_ready_to_manipulate;

public:
    Camera *cam1, *cam2;
    PictureManipulator *pm;

private:
    bool openCamera(Camera *cam);

public:
    Controller(PictureManipulator *pmr, Camera *camera1, Camera *camera2 = nullptr);
    ~Controller();

    void getImageFromCamera();
    bool startReceiveImageThread(Camera *cam);
    bool mainpulatePicture();
    bool config(string serial_port,
                string path,
                double fps,
                int width,
                int height,
                int offset_x,
                int offset_y,
                double expotime,
                int64_t gain);
    void adjustParameter();
};

#endif