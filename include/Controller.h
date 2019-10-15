/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 14:23:07 +0800
 * @LastEditTime: 2019-10-14 20:39:30 +0800
 * @LastEditors: 
 * @Description: 改文件只允许main进行引用
 */
#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "header.h"
#include "actions.h"
#include "PictureManipulator.h"
#include "Camera.h"

class Controller
{
private:
    bool is_get_image = true;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    int is_ready_to_manipulate = 0;
    Mat source_img;
    Mat img_ready_to_manipulate;

public:
    Camera *cam;
    PictureManipulator *pm;

private:
public:
    Controller(Camera *camera, PictureManipulator *pmr);
    ~Controller();

    void getImageFromCamera();
    bool startReceiveImageThread(Camera *cam);
    bool mainpulatePicture();
};

#endif