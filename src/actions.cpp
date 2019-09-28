/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-09-09 21:10:49 +0800
 * @LastEditTime: 2019-09-09 21:11:57 +0800
 * @LastEditors: 
 * @Description: 
 */
#include "header.h"
#include "actions.h"



/**
 * @Author: 王占坤
 * @Description: 显示图片
 * @Param: string name 显示窗口名称
 * @Param: picture 需要显示的图片
 * @Param: time 显示时间，单位秒
 * @Return: void
 * @Throw: 
 */
void showPicture(string window_name, const Mat& picture, double time)
{
    imshow(window_name, picture);
    waitKey(time*1000);
}



void GetErrorString(GX_STATUS error_status)
{
    char *error_info = NULL;
    size_t size = 0;
    GX_STATUS status = GX_STATUS_SUCCESS;

    // 获取错误描述信息长度
    status = GXGetLastError(&error_status, NULL, &size);
    if (status != GX_STATUS_SUCCESS)
    {
        cout << "<GXGetLastError call fail>" << endl;
        return;
    }

    error_info = new char[size];
    if (error_info == NULL)
    {
        printf("<Failed to allocate memory>\n");
        return;
    }

    // 获取错误信息描述
    status = GXGetLastError(&error_status, error_info, &size);
    if (status != GX_STATUS_SUCCESS)
    {
         cout << "<GXGetLastError call fail>" << endl;
        return;
    }
    else
    {
        printf("%s\n", (char *)error_info);
    }

    // 释放资源
    if (error_info != NULL)
    {
        delete[] error_info;
        error_info = NULL;
    }
}