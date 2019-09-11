/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:35:15 +0800
 * @LastEditTime: 2019-08-31 10:35:15 +0800
 * @LastEditors: 
 * @Description: 
 */
#ifndef SERIAL_H
#define SERIAL_H

#include "Header.h"


//@brief:
//@brief:linux下的串口通信类，可以通过构造函数直接打开一个串口，并初始化（默认9600波特率，8位数据，无奇偶校验，1位停止位）
//             send()成员函数可以直接发送字符串，set_opt()更改参数。串口会在析构函数中自动关闭
//@example:Serialport exp("/dev/ttyUSB0");
//                  exp.set_opt(115200,8,'N',1);
//                  exp.send("1123dd");
class Serialport
{
public:
        Serialport(string port);//定义Serialport类的成员函数，
        Serialport();
        ~ Serialport();
        int open_port(string port);
        int set_opt(int nSpeed = 115200 , int nBits = 8, char nEvent ='N', int nStop = 1);
        bool send(char *str);
        bool sendAngle(float angleYaw,float anglePitch,float Dis, bool big, bool insight ,bool get);

        void readMode(int &carMode);
        uint8_t readAngle(CarData &cardata);

private:
         int fd ;
         char tmpchar[19];
         const char *buffer;
         unsigned char rData[255];
         char guapichar[4];
};

#endif
