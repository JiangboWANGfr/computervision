/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 19:35:43 +0800
 * @LastEditTime: 2019-09-28 14:19:36 +0800
 * @LastEditors: 
 * @Description: 
 */

#include "header.h"
#include "SerialPort.hpp"
#include "DxImageProc.h"
#include "GxIAPI.h"
#include "Camera.h"
#include "PictureManipulator.h"
#include "SentryPictureManipulator.h"
/////////////////全局变量声明区//////////////
Camera cam;

queue<Mat> image_queue;
////////////////////函数声明//////////////
bool startReceiveImageThread();
void *getImageFromCamera(void *pParam);
void terminate_program();
///////////////////main///////////////

int main()
{
    SentryPictureManipulator spm;
    PictureManipulator *pm = &spm;
    cout << "Initializion.........." << endl;
    
    cam.open();
    if(cam.is_opened == false)
    {
        cout << "Faile to open camera." << endl;
        return 0;
    }
    
    cam.configFrame();
    cout << "Success to config source Image" << endl;
    if (startReceiveImageThread() == false)
        return 0;

    // mainpulatePicture();
    while (true)
    {
        char a;
        cin >> a;
        if (image_queue.empty())
            continue;
        pm->manipulatePicture(image_queue.front());
        image_queue.pop();
    }

    terminate_program();
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
    cam.start();
    while (true)
    {
        image_queue.push(cam.getFrame());
    }
}

void terminate_program()
{
    cam.close();
}