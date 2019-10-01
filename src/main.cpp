/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 19:35:43 +0800
 * @LastEditTime: 2019-09-30 23:24:52
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
#include "for_main.h"
/////////////////全局变量声明区//////////////
Camera cam;

queue<Mat> image_queue;
///////////////////main///////////////

int main()
{
    SentryPictureManipulator spm("/tty/USB0",
                                 "../build/",
                                 60,
                                 640,
                                 480);
    cout << "Initializion.........." << endl;

    cam.open();
    if (cam.is_opened == false)
    {
        cout << "Faile to open camera." << endl;
        return 0;
    }

    cam.configFrame();
    cout << "Success to config source Image" << endl;
    if (startReceiveImageThread(cam) == false)
        return 0;

    mainpulatePicture(&spm);

    terminate_program(cam);
}
