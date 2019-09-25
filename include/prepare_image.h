/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-25 16:52:38 +0800
 * @LastEditTime: 2019-09-25 17:17:07 +0800
 * @LastEditors: 
 * @Description: 
 */
#ifndef __PREPARE_IMAGE_H__
#define __PREPARE_IMAGE_H__

#include "header.h"

int configSourceImage(GX_DEV_HANDLE& camera_hd);


bool startReceiveImageThread();


#endif