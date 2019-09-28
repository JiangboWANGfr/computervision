/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 19:35:43 +0800
 * @LastEditTime: 2019-09-28 15:43:34 +0800
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
    if (startReceiveImageThread(cam) == false)
        return 0;

    // mainpulatePicture();
    while (true)
    {
        if (image_queue.empty())
            continue;
        pm->manipulatePicture(image_queue.front());
        image_queue.pop();
    }

    terminate_program(cam);
}
