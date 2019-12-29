#ifndef __VIRTUAL_CAMERA_H__

#include "header.h"
#include "Camera.h"

#include "stdlib.h"

class VirtualCamera : public Camera
{
private:
    VideoCapture cam;
public:
    VirtualCamera(string cam); 
    ~VirtualCamera(); 
    int open();
    int configFrame(int64_t width,
                    int64_t height,
                    int offset_x,
                    int offset_y ,
                    double expotime,
                    int64_t gain);
    int close();
    int start();
    Mat getFrame();
};






#endif