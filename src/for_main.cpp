/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 14:23:00 +0800
 * @LastEditTime: 2019-09-30 22:54:14
 * @LastEditors: 
 * @Description: 
 */
#include "for_main.h"

bool startReceiveImageThread(Camera& cam)
{
    //启动接收线程
    pthread_t tid;
    int ret = pthread_create(&tid, 0, getImageFromCamera, &cam);
    if (ret != 0)
    {
        printf("<Failed to create the collection thread>\n");
        cam.close();
        return false;
    }
    cout << "Success to create the collection thread\n";
    return true;
}

void *getImageFromCamera(void *arg)
{
    Camera *cam = (Camera*)arg;
    pthread_detach(pthread_self());
    cout << cam->start() << endl;
    cout << "\n\n\n\ngetImageFromCamera\n\n\n\n";

    while (true)
    {
        image_queue.push(cam->getFrame());
        showPicture("source from queue",image_queue.front(),0.001);
        cout << "Number of images of queue: " << image_queue.size() << endl;
    }
}

void terminate_program(Camera &cam)
{
    cam.close();
}

void mainpulatePicture(PictureManipulator* pm)
{
    while (true)
    {
        cout << "mainpulate picture" << endl;
        if (image_queue.empty())
            continue;
        pm->manipulatePicture(image_queue.front());
        image_queue.pop();
    }
}