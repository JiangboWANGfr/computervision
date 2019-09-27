/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 19:35:43 +0800
 * @LastEditTime: 2019-09-25 17:33:14 +0800
 * @LastEditors: 
 * @Description: 
 */

#include "header.h"
#include "prepare_camera.h"
#include "manipulate_picture.h"
#include "Serial.hpp"
#include "DxImageProc.h"
#include "GxIAPI.h"
#include "terminate_program.h"
#include "prepare_image.h"
/////////////////全局变量声明区//////////////
GX_DEV_HANDLE camera_hd = NULL; ///< 相机句柄
queue<Mat> image_queue;


///////////////////main///////////////

int main()
{
    cout << "Initializion.........." << endl;
    prepareCamera(camera_hd);
    cout << "camera_handle: " << camera_hd << endl;
    cout << "Success to open camera" << endl;
    configSourceImage(camera_hd);
    cout << "Success to config source Image" << endl;
    if (startReceiveImageThread() == false)
        return 0;

    mainpulatePicture();

    terminateProgram(camera_hd);
}

