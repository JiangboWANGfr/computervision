/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 19:35:43 +0800
 * @LastEditTime: 2019-10-18 14:52:57 +0800
 * @LastEditors: 
 * @Description: 
 */

#include "config.h"
#include "header.h"
#include "SerialPort.hpp"
#include "DxImageProc.h"
#include "GxIAPI.h"
#include "GxCamera.h"
#include "OrdinaryCamera.h"
#include "PictureManipulator.h"
#include "SentryPictureManipulator.h"
#include "Controller.h"
#include "InfantryPictureManipulator.h"
#pragma comment(linker, "/STACK:102400000,102400000")
pthread_attr_t thread_attr;
void *startReceiveImageThread(void *ctrl);
void *startManipulatePictureThread(void *ctrl);
void *mainThreadOne(void *argc);
void *mainThreadTwo(void *argc);
int terminateProgram();

#ifdef SENTRY

int main()
{
    //设置线程参数
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&thread_attr, PTHREAD_STACK_MIN * 512);

    pthread_t t;
    // pthread_create(&t, &thread_attr, mainThreadOne, NULL);
    pthread_create(&t, &thread_attr, mainThreadTwo, NULL);

    char tmp;
    cin >> tmp; //阻塞
    cout << "GAME OVER!" << endl;
    pthread_exit(NULL);
    terminateProgram();
}

#endif

void *mainThreadOne(void *argc)
{
    OrdinaryCamera cam2("/dev/video0");
    SentryPictureManipulator pm;
    GxCamera cam1("1");
    // Controller controller(&pm,&cam2);
    // Controller controller(&pm, &cam1, &cam2);
    Controller controller(&pm, &cam1);
    controller.config("/tty/USB0", "./", 120, 640, 480, 6, 8, 1200, 200);

    pthread_t ri_th, mp_th;

    int err = pthread_create(&mp_th, &thread_attr, startManipulatePictureThread, &controller);

    if (err != 0)
    {
        cout << "main:: startManipulatePictureThread failed" << endl;
        printf("error message :%s\n",
               strerror(errno));
    }
    //  启动接收线程
    err = pthread_create(&ri_th, &thread_attr, startReceiveImageThread, &controller);
    if (err != 0)
    {
        cout << "main:: startReceiveImageThread failed" << endl;
        printf("error message :%s\n",
               strerror(errno));
    }
    char tmp;
    cin >> tmp; //用于阻塞
}

void *mainThreadTwo(void *argc)
{
    OrdinaryCamera cam2("/dev/video0");
    SentryPictureManipulator pm;
    GxCamera cam1("1");
    Controller controller(&pm, &cam2);
    // Controller controller(&pm, &cam1, &cam2);
    // Controller controller(&pm,&cam1);
    controller.config("/tty/USB0", "./", 120, 640, 480, 6, 8, 1200, 200);

    pthread_t ri_th, mp_th;

    int err = pthread_create(&mp_th, &thread_attr, startManipulatePictureThread, &controller);

    if (err != 0)
    {
        cout << "main:: startManipulatePictureThread failed" << endl;
        printf("error message :%s\n",
               strerror(errno));
    }
    //  启动接收线程
    err = pthread_create(&ri_th, &thread_attr, startReceiveImageThread, &controller);
    if (err != 0)
    {
        cout << "main:: startReceiveImageThread failed" << endl;
        printf("error message :%s\n",
               strerror(errno));
    }
    char tmp;
    cin >> tmp; //用于阻塞
}

void *startReceiveImageThread(void *ctrl)
{
    pthread_detach(pthread_self());
    // cout << "startReceiveImageThread" << endl;
    Controller *controller = (Controller *)ctrl;
    while (true)
    {
        controller->getImageFromCamera();
    }
}

void *startManipulatePictureThread(void *ctrl)
{
    pthread_detach(pthread_self());
    Controller *controller = (Controller *)ctrl;
    while (true)
    {
        controller->mainpulatePicture();
    }
}

int terminateProgram()
{
    // cam1.close();
    // cam2.close();
    return 0;
}