/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-28 14:23:07 +0800
 * @LastEditTime: 2019-10-01 22:06:30 -0700
 * @LastEditors: 
 * @Description: 改文件只允许main进行引用
 */
#ifndef __FOR_MAIN_H__
#define __FOR_MAIN_H__

#include "header.h"
#include "actions.h"
#include "PictureManipulator.h"
#include "Camera.h"

bool startReceiveImageThread(Camera* cam);
void terminateProgram(Camera *cam);
bool mainpulatePicture(PictureManipulator* pm);



#endif