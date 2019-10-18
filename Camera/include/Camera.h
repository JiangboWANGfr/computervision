/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-27 19:52:54 +0800
 * @LastEditTime: 2019-10-18 12:59:31 +0800
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
    string cam_name;
public:
    void* camera_handle = NULL; ///< 相机句柄
    bool is_opened = false;

public:
    Camera(string camera_name):cam_name(camera_name){}
    ~Camera(){}
    virtual int open() = 0;//打开相机
    virtual int configFrame(int64_t width,
                    int64_t height,
                    int offset_x,
                    int offset_y ,
                    double expotime,
                    int64_t gain) = 0;//对相机进行配置
    virtual int close() = 0;//关闭相机
    virtual int start() = 0;//启动相机拍摄
    virtual Mat getFrame() = 0;//获取相机图片
};

#endif