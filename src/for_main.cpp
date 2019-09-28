/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 14:23:00 +0800
 * @LastEditTime: 2019-09-28 14:29:54 +0800
 * @LastEditors: 
 * @Description: 
 */
#include "for_main.h"

bool startReceiveImageThread(Camera& cam)
{
    //启动接收线程
    pthread_t tid;
    int ret = pthread_create(&tid, 0, getImageFromCamera, 0);
    if (ret != 0)
    {
        printf("<Failed to create the collection thread>\n");
        cam.close();
        return false;
    }
    return true;
}

void *getImageFromCamera(void *arg)
{
    Camera *cam = (Camera*)arg;
    pthread_detach(pthread_self());
    GX_STATUS status = GX_STATUS_SUCCESS;
    cam->start();
    while (true)
    {
        image_queue.push(cam->getFrame());
    }
}

void terminate_program(Camera &cam)
{
    cam.close();
}