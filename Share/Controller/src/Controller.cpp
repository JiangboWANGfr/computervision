/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 14:23:00 +0800
 * @LastEditTime : 2019-12-23 21:48:33
 * @LastEditors  : zzdr
 * @Description: 
 */

#include "Controller.h"

#include <chrono> //用于计算时间

//初始化静态变量以及为静态变量分配空间
int Controller::num_of_controller = 0;
pthread_mutex_t Controller::s_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t Controller::con_sem;

#ifdef SOCKET_COMMUNICATION
Socket Controller::client(SOCK_STREAM, 65535, "10.139.13.46");
#endif
#ifdef STM32
SerialPort Controller::stm32;
#endif

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
    : pm(pmr), cam1(camera1), cam2(camera2)
{
    pthread_mutex_lock(&s_mutex);
    if (num_of_controller == 0) //避免重复初始化
    {
        sem_init(&con_sem, 0, 0);
    }
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
    source_img = cam1->getFrame();
    pthread_mutex_lock(&mutex);
    if (is_ready_to_manipulate == 0)
    {
        auto clone_start = chrono::system_clock::now();
#ifdef SAVE_DATA
        src_video << source_img; //保存原始图像
#endif
        img_ready_to_manipulate = source_img.clone();
        is_ready_to_manipulate = 1;
        auto clone_end = chrono::system_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(clone_end - clone_start);
        // cout << "Clone time: " << double(duration.count()) * chrono::microseconds::period::num / chrono::microseconds::period::den * 1000 << "ms" << endl;
        // cout << controller_handle << endl;
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
        auto manipulate_start = chrono::system_clock::now();

        pm->manipulatePicture(img_ready_to_manipulate);

        auto manipulate_end = chrono::system_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(manipulate_end - manipulate_start);
        // cout << "ManipulatePicture Time: " << double(duration.count()) * chrono::microseconds::period::num / chrono::microseconds::period::den * 1000 << "ms" << endl;

#ifdef SHOW_PICTURE
        showPicture("after", img_ready_to_manipulate, 1);
#endif
#ifdef SAVE_DATA
        fin_video << img_ready_to_manipulate; //保存处理后的图像
#endif
        is_ready_to_manipulate = 0;
        sem_post(&Controller::con_sem);
    }
    pthread_mutex_unlock(&mutex);
}

void Controller::sendMSG()
{
    auto manipulate_start = chrono::system_clock::now();
    sem_wait(&Controller::con_sem);
    cJSON *j = cJSON_CreateObject();
    pm->armor_data.toJson(j);
    cout << "Controller::sendMSG\n" << cJSON_Print(j) << endl;
#ifdef SOCKET_COMMUNICATION
    client.sendToServer(cJSON_Print(j));
#endif

#ifdef STM32
    stm32.send(pm->armor_data);
#endif
    auto manipulate_end = chrono::system_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(manipulate_end - manipulate_start);
    cout << "sendMSG Time: " << double(duration.count()) * chrono::microseconds::period::num / chrono::microseconds::period::den * 1000 << "ms" << endl;
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
#ifdef STM32
    stm32.open_port(serial_port);
    stm32.set_opt(115200, 8, 'N', 1);
#endif

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