/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 19:35:43 +0800
 * @LastEditTime: 2019-10-01 22:06:13 -0700
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

queue<Mat> image_queue;

///////////////////main///////////////

int main()
{
    Camera cam;

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

    cam.configFrame(640,480,6,8,1200,200);
    cout << "Success to config source Image" << endl;
    if (startReceiveImageThread(&cam) == false)
        return 0;

    cout << "main::    mainpulate picture" << endl;
    mainpulatePicture(&spm);

    terminateProgram(&cam);
}
