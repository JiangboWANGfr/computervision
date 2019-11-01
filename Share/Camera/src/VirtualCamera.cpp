#include "VirtualCamera.h"

/**
 * @Author: 王占坤
 * @Description: 创建一个打开视频的虚拟相机
 * @Param: string video_path 需要打开的视频路径
 * @Return: 
 * @Throw: 
 */
VirtualCamera::VirtualCamera(string video_path) : Camera(video_path)
{
    this->cam_name = video_path;
}

VirtualCamera::~VirtualCamera()
{
}

int VirtualCamera::open()
{
    cam.open(cam_name);
    this->is_opened = cam.isOpened();
}
/**
 * @Author: 王占坤
 * @Description: 
 * @Param: 
 * @Return: 
 * @Throw: 
 */
int VirtualCamera::configFrame(int64_t width,
                               int64_t height,
                               int offset_x,
                               int offset_y,
                               double expotime,
                               int64_t gain)
{
}

int VirtualCamera::close()
{}

int VirtualCamera::start()
{}

Mat VirtualCamera::getFrame()
{
    Mat frame;
    cam.read(frame);
    return frame;
}
