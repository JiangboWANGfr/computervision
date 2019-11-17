/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 14:23:00 +0800
 * @LastEditTime: 2019-11-17 15:13:00 +0800
 * @LastEditors: 
 * @Description: 
 */

#include "Controller.h"

int Controller::num_of_controller = 0;
pthread_mutex_t Controller::s_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @Author: 王占坤
 * @Description: 至少要有一个相机输入
 * @Param: PictureManipulator *pmr  处理图片的类，输入为地址
 * @Param: Camera *camera1  工业相机1或者普通相机
 * @Param: Camera *camera2 工业相机2或者是普通相机，默认值为nullptr
 * @Return: 
 * @Throw: 
 */
Controller::Controller(PictureManipulator *pmr, Camera *camera1, Camera *camera2)
    : pm(pmr), cam1(camera1), cam2(camera2), client(SOCK_DGRAM, 8888, "127.0.0.1")
{
    pthread_mutex_lock(&s_mutex);
    num_of_controller++;
    controller_handle = num_of_controller;
    pthread_mutex_unlock(&s_mutex);
    bool is_opened = openCamera(cam1);
    if (is_opened == false)
    {
        cout << "Controller::config Failed to open camera" << endl;
        exit(-1);
    }
    cout << "Controller::Controller    camera handle:    " << cam1->camera_handle << endl;
    if (cam2 != nullptr)
    {
        is_opened = openCamera(cam2);
        if (is_opened == false)
        {
            cout << "Controller::config Failed to open cam1" << endl;
        }
    }
}

Controller::~Controller()
{
    //这里实际上需要实现num_of_controller,但是一旦需要实现，很多
    //都需要进行更改，否则controller_handle就会出错
    //因此不再实现
}

/**
 * @Author: 王占坤
 * @Description: 
 * @Param: void* arg 传入的是相机类
 * @Return: 
 * @Throw: 
 */
void Controller::getImageFromCamera()
{

    double start = clock();
    pthread_mutex_lock(&mutex);
    source_img = cam1->getFrame();
    if (is_ready_to_manipulate == 0)
    {
        double clone_start = clock();
#ifdef SAVE_DATA
        src_video << source_img; //保存原始图像
#endif
        img_ready_to_manipulate = source_img.clone();
        is_ready_to_manipulate = 1;
        double clone_end = clock();
        cout << "Clone time: " << (clone_end - clone_start) * 1000 / CLOCKS_PER_SEC << "ms" << endl;
        cout << controller_handle << endl;
    }
#ifdef SHOW_PICTURE
    showPicture("Controller::getImageFromCamera  img_ready_to_manipulate" + to_string(Controller::controller_handle), img_ready_to_manipulate, 1);
#endif
    pthread_mutex_unlock(&mutex);
}

bool Controller::mainpulatePicture()
{
    pthread_mutex_lock(&mutex);
    if (is_ready_to_manipulate == 1)
    {
        pm->manipulatePicture(img_ready_to_manipulate);
        showPicture("after", img_ready_to_manipulate, 1);
#ifdef SAVE_DATA
        fin_video << img_ready_to_manipulate; //保存处理后的图像
#endif
        is_ready_to_manipulate = 0;
        client.sendToServer((char *)&pm->armor_data);
#ifdef STM32
        stm32.send(pm->armor_data);
#endif
    }
    pthread_mutex_unlock(&mutex);
}

bool Controller::config(string serial_port,
                        string path,
                        double fps,
                        int width,
                        int height,
                        int offset_x,
                        int offset_y,
                        double expotime,
                        int64_t gain)
{
    cam1->configFrame(width, height, offset_x, offset_y, expotime, gain);
    cam1->start();
    if (cam2 != nullptr)
    {
        cam2->configFrame(width, height, offset_x, offset_y, expotime, gain);
        cam2->start();
    }

    stm32.open_port(serial_port);

    filename = path;
    if (*filename.end() == '/')
    {
        filename += std::to_string(time(NULL));
    }
    else
    {
        filename += '/' + std::to_string(time(NULL));
    }

    video_size.height = height;
    video_size.width = width;

    src_video.open(filename + "SRC.avi", CV_FOURCC('M', 'J', 'P', 'G'), fps, video_size);
    fin_video.open(filename + "FIN.avi", CV_FOURCC('M', 'J', 'P', 'G'), fps, video_size);
}

void Controller::adjustParameter()
{
    string window_name = "Parameter";
    namedWindow(window_name);
    createTrackbar("Armordetector.gray_thresh", window_name, &(pm->armor_detector.gray_thresh), 255);
    createTrackbar("ArmorDetector.single_color_img", window_name, &(pm->armor_detector.single_color_thresh), 255);
}

bool Controller::openCamera(Camera *cam)
{
    cam->open();
    return cam->is_opened;
}