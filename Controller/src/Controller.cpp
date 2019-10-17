/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 14:23:00 +0800
 * @LastEditTime: 2019-10-17 21:01:28 +0800
 * @LastEditors: 
 * @Description: 
 */

#include "Controller.h"

Controller::Controller(Camera *camera, PictureManipulator *pmr)
    : cam(camera), pm(pmr)
{
    // pthread_mutex_init(&mutex);
    pm->adjustParameter();
    cam->start();
    cout << "Controller::Controller    camera handle:    " << cam->camera_handle << endl;
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
        img_ready_to_manipulate = source_img.clone();
        is_ready_to_manipulate = 1;
        double clone_end = clock();
        cout << "Clone time: " << (clone_end - clone_start) * 1000 / CLOCKS_PER_SEC << "ms" << endl;
    }
    showPicture("hello", img_ready_to_manipulate, 0.001);
    pthread_mutex_unlock(&mutex);
}

bool Controller::mainpulatePicture()
{
    pthread_mutex_lock(&mutex);
    if (is_ready_to_manipulate == 1)
    {
        pm->manipulatePicture(img_ready_to_manipulate);
        is_ready_to_manipulate = 0;
    }
    pthread_mutex_unlock(&mutex);
}
