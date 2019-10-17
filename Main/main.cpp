/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 19:35:43 +0800
 * @LastEditTime: 2019-10-17 18:56:21 +0800
 * @LastEditors: 
 * @Description: 
 */

#include "header.h"
#include "SerialPort.hpp"
#include "DxImageProc.h"
#include "GxIAPI.h"
#include "GxCamera.h"
#include "PictureManipulator.h"
#include "SentryPictureManipulator.h"
#include "Controller.h"
#pragma comment(linker, "/STACK:102400000,102400000") 
GxCamera cam;

SentryPictureManipulator spm("/tty/USB0",
                             "../build/",
                             60,
                             640,
                             480);

void *startReceiveImageThread(void *ctrl);
void *startManipulatePictureThread(void *ctrl);
int terminateProgram();

///////////////////main///////////////

int main()
{

    cout << "Initializion.........." << endl;

    cam.open();
    if (cam.is_opened == false)
    {
        cout << "Faile to open camera." << endl;
        return 0;
    }

    cam.configFrame(640, 480, 6, 8, 1200, 200);

    
    Controller controller(&cam, &spm); //必须要在配置好camera之后再生成该变量

    pthread_t ri_th, mp_th;
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&thread_attr, PTHREAD_STACK_MIN * 1024);


    int err = pthread_create(&mp_th, &thread_attr, startManipulatePictureThread, &controller);

    if (err != 0)
    {
        cout << "main:: startManipulatePictureThread failed" << endl;
        printf("error message :%s\n",
               strerror(errno));
        return -1;
    }
    //  启动接收线程
     err = pthread_create(&ri_th, &thread_attr, startReceiveImageThread, &controller);
    if (err != 0)
    {
        cout << "main:: startReceiveImageThread failed" << endl;
        printf("error message :%s\n",
               strerror(errno));
        // return -1;
    }

    pthread_join(ri_th, NULL);
    pthread_join(mp_th, NULL);
    int tmp;
    cin >> tmp;
    cout << "GAME OVER!" << endl;
    pthread_exit(NULL);
    terminateProgram();
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
    cam.close();
    return 0;
}