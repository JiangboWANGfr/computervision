/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-27 19:52:54 +0800
 * @LastEditTime: 2019-10-17 23:26:11 +0800
 * @LastEditors: 
 * @Description: 
 */
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "header.h"
#include "actions.h"

class Camera
{
protected:

public:
    void* camera_handle = NULL; ///< 相机句柄
    bool is_opened = false;

public:
    Camera(/* args */){}
    ~Camera(){}
    virtual int open() = 0;
    virtual int configFrame(int64_t width,
                    int64_t height,
                    int offset_x,
                    int offset_y ,
                    double expotime,
                    int64_t gain) = 0;
    virtual int close() = 0;
    virtual int start() = 0;
    virtual Mat getFrame() = 0;
};

#endif