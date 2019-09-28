/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 19:35:43 +0800
 * @LastEditTime: 2019-09-28 11:24:36 +0800
 * @LastEditors: 
 * @Description: 
 */

#include "header.h"
#include "manipulate_picture.h"
#include "Serial.hpp"
#include "DxImageProc.h"
#include "GxIAPI.h"
// #include "prepare_image.h"
#include "Camera.h"
/////////////////全局变量声明区//////////////
Camera cam;

queue<Mat> image_queue;
////////////////////函数声明//////////////
bool startReceiveImageThread();
void *getImageFromCamera(void *pParam);
///////////////////main///////////////

int main()
{
    cout << "Initializion.........." << endl;
    cam.open();
    cout << "camera_handle: " << cam.camera_handle << endl;
    cout << "Success to open camera" << endl;
    cam.configFrame();
    cout << "Success to config source Image" << endl;
    if (startReceiveImageThread() == false)
        return 0;

    mainpulatePicture();

    cam.close();
}


bool startReceiveImageThread()
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

void *getImageFromCamera(void *pParam)
{
    pthread_detach(pthread_self());
    GX_STATUS status = GX_STATUS_SUCCESS;
    //接收线程启动标志
    // g_get_image = true;
    //发送开采命令
    cam.start();
    while (true)
    {
        image_queue.push(cam.getFrame());
    }
}