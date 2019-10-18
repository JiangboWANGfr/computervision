/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 14:23:00 +0800
 * @LastEditTime: 2019-10-18 09:49:44 +0800
 * @LastEditors: 
 * @Description: 
 */

#include "Controller.h"

Controller::Controller(Camera *camera, PictureManipulator *pmr)
    : cam(camera), pm(pmr)
{
}

Controller::~Controller()
{
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
    source_img = cam->getFrame();

    pthread_mutex_lock(&mutex);
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
    }
#ifdef SHOW_PICTURE
    showPicture("COntroller::getImageFromCamera  img_ready_to_manipulate", img_ready_to_manipulate, 1);
#endif
    pthread_mutex_unlock(&mutex);
}

bool Controller::mainpulatePicture()
{
    pthread_mutex_lock(&mutex);
    if (is_ready_to_manipulate == 1)
    {
        pm->manipulatePicture(img_ready_to_manipulate);
        // fin_video << img_ready_to_manipulate; //保存处理后的图像
        is_ready_to_manipulate = 0;
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
    stm32.open_port(serial_port);

    cam->open();
    if (cam->is_opened == false)
    {
        cout << "Faile to open camera." << endl;
        return false;
    }
    cam->configFrame(width, height, offset_x, offset_y, expotime, gain);
    cam->start();
    cout << "Controller::Controller    camera handle:    " << cam->camera_handle << endl;

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