/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 19:35:43 +0800
 * @LastEditTime: 2019-11-17 15:04:04 +0800
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
#include "VirtualCamera.h"
#include "PictureManipulator.h"
#include "Controller.h"
#include "InfantryPictureManipulator.h"
#include "SentryPictureManipulator.h"
#include "HeroPictureManipulator.h"

#pragma comment(linker, "/STACK:102400000,102400000")

pthread_attr_t thread_attr;
void *startReceiveImageThread(void *ctrl);
void *startManipulatePictureThread(void *ctrl);
void *mainThreadOne(void *argc);
void *mainThreadTwo(void *argc);
int terminateProgram();

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

void *mainThreadOne(void *argc)
{
    OrdinaryCamera cam2("/dev/video0");
#ifdef SENTRY
    SentryPictureManipulator pm;
#endif
#ifdef INFANTRY
    InfantryPictureManipulator pm;
#endif
#ifdef Hero
    HeroPictureManipulator pm;
#endif
    GxCamera cam1("1");
    // Controller controller(&pm,&cam2);
    // Controller controller(&pm, &cam1, &cam2);
    Controller controller(&pm, &cam1);
    controller.config("/tty/USB0", "./", 120, 1280, 1024, 6, 8, 1500, 200);

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
    // GxCamera cam2("1");
    OrdinaryCamera cam2("/dev/video0");
// VirtualCamera cam2("./1572528438SRC.avi");
#ifdef SENTRY
    SentryPictureManipulator pm;
#endif
#ifdef INFANTRY
    InfantryPictureManipulator pm;
#endif
#ifdef Hero
    HeroPictureManipulator pm;
#endif
    Controller controller(&pm, &cam2);
    // Controller controller(&pm, &cam1, &cam2);
    // Controller controller(&pm,&cam1);
    controller.config("/tty/USB0", "./", 120, 1280, 1024, 6, 8, 1200, 200);

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