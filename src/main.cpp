/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 19:35:43 +0800
 * @LastEditTime: 2019-09-28 10:56:48 +0800
 * @LastEditors: 
 * @Description: 
 */

#include "header.h"
#include "manipulate_picture.h"
#include "Serial.hpp"
#include "DxImageProc.h"
#include "GxIAPI.h"
#include "prepare_image.h"
#include "Camera.h"
/////////////////全局变量声明区//////////////
Camera cam;

queue<Mat> image_queue;

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
