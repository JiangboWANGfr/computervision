/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 14:23:00 +0800
 * @LastEditTime: 2019-10-01 22:08:25 -0700
 * @LastEditors: 
 * @Description: 
 */
#include "for_main.h"

pthread_t ri_id;//接受图片的线程id
bool is_get_image = true;

void *getImageFromCamera(void *arg);

bool startReceiveImageThread(Camera *cam)
{
    //启动接收线程
    int ret = pthread_create(&ri_id, 0, getImageFromCamera, cam);
    if (ret != 0)
    {
        printf("<Failed to create the collection thread>\n");
        terminateProgram(0);
        return false;
    }
    cout << "Success to create the collection thread\n";
    return true;
}

/**
 * @Author: 王占坤
 * @Description: 
 * @Param: void* arg 传入的是相机类
 * @Return: 
 * @Throw: 
 */
void *getImageFromCamera(void *arg)
{
    Camera *cam = (Camera *)arg;
    // pthread_detach(pthread_self());
    cout << cam->start() << endl;

    while (is_get_image)
    {
        double start = clock();
        image_queue.push(cam->getFrame());
        // cout << "Number of images of queue: " << image_queue.size() << endl;
        double end = clock();
        // std::cout << "Get Frame Time Per Pic:" << (end - start) / CLOCKS_PER_SEC * 1000 << "ms\n"
        //   << endl;
    }
}

/**
 * @Author: 王占坤
 * @Description: 终止程序的操作
 * @Param: 
 * @Return: 
 * @Throw: 
 */
void terminateProgram(Camera* cam)
{
    pthread_join(ri_id, NULL);
    cam->close();
}

bool mainpulatePicture(PictureManipulator *pm)
{
    while (true)
    {
        cout << "for_main::mainpulatePicture    mainpulate picture" << endl; //不能被注释掉，注释可能不会执行该函数，原因未知
        if (image_queue.empty())
            continue;
        pm->manipulatePicture(image_queue.front());
        showPicture("for_main::mainpulatePicture    source image from queue", image_queue.front(), 0.001);
        image_queue.pop();
    }
}
