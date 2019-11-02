/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-10-12 17:50:55
 * @LastEditTime: 2019-11-02 10:10:27
 * @LastEditors: Please set LastEditors
 */
#include <opencv2/opencv.hpp>
#include <thread>
#include <unistd.h>
#include "BuffDetector.h"

using namespace std;
using namespace cv;

int main() { 

    BuffDetectorWrapper buff_detector;
    
    VideoCapture cap("../Videos/Sivir2.avi");
    Mat image; 
    float angle_x = 0.0, angle_y = 0.0, distance =  0.0;
    int command = 0;
    TargetData armor_data; 

    while (1) { 
        bool status = cap.read(image);
        if (!status) 
            break; 

        imshow("rawimg", image); 
        buff_detector.getCenter(image, armor_data); 
        printf("Distance: %f, Yaw: %f, Pitch: %f\n", armor_data.atocDistance, armor_data.yaw_angle, armor_data.pitch_angle); 
        waitKey(1); 
    }
}