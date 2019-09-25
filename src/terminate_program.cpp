/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-25 16:35:12 +0800
 * @LastEditTime: 2019-09-25 17:20:57 +0800
 * @LastEditors: 
 * @Description: 
 */
#include "terminate_program.h"
#include "actions.h"

int UnPreForImage(GX_DEV_HANDLE &camera_handle);


int terminateProgram(GX_DEV_HANDLE &camera_handle)
{
    //为停止采集做准备
    int ret = UnPreForImage(camera_handle);
    //关闭设备
    GX_STATUS status = GXCloseDevice(camera_handle);
    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
        if (camera_handle != NULL)
        {
            camera_handle = NULL;
        }
        status = GXCloseLib();
        return 0;
    }
    //释放库
    else
        status = GXCloseLib();

    return 0;
}


int UnPreForImage(GX_DEV_HANDLE &camera_handle)
{
    GX_STATUS status = GX_STATUS_SUCCESS;
    uint32_t ret = 0;

    //发送停采命令
    status = GXSendCommand(camera_handle, GX_COMMAND_ACQUISITION_STOP);
    if (status != GX_STATUS_SUCCESS)
    {
        GetErrorString(status);
        return status;
    }

    // g_get_image = false;
    // // ret = pthread_join(g_acquire_thread, NULL);
    // if (ret != 0)
    // {
    //     printf("<Failed to release resources>\n");
    //     return ret;
    // }

    // //释放buffer
    // if (g_frame_data.pImgBuf != NULL)
    // {
    //     free(g_frame_data.pImgBuf);
    //     g_frame_data.pImgBuf = NULL;
    // }

    return 0;
}