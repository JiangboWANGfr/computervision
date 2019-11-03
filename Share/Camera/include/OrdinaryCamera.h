#ifndef __ORDINARY_CAMERA_H__
#define __ORDINARY_CAMERA_H__

#include "header.h"
#include "actions.h"
#include "Camera.h"

#ifdef ORDINARY_CAMERA

class OrdinaryCamera : public Camera
{
private:
    VideoCapture ord_cam;
    Mat img;
public:
    OrdinaryCamera(string camera_name);
    ~OrdinaryCamera();

    int open();
    //普通免驱相机不实现
    int configFrame(int64_t width,
                    int64_t height,
                    int offset_x,
                    int offset_y ,
                    double expotime,
                    int64_t gain){}
    int close(){}
    int start(){}
    Mat getFrame();
};

#endif

#endif