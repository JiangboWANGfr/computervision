<!--
 * @Description: In User Settings Edit
 * @Author: 王占坤
 * @Date: 2019-09-30 23:55:43
 * @LastEditTime: 2019-09-30 23:57:42
 * @LastEditors: 
 -->
# 文件说明
## include文件夹
```
include
├── actions.h
├── AngleCalculate.hpp
├── ArmorDetector.hpp
├── Camera.h
├── config.h
├── DxImageProc.h
├── for_main.h
├── GxIAPI.h
├── header.h
├── PictureManipulator.h
├── RMVideoCapture.hpp
├── SentryPictureManipulator.h
└── SerialPort.hpp
```
- actions.h 声明常用文件，所有文件都需要include该文件，header.h除外
- AngleCalculate.h 该文件无所谓，是否有自己决定
- ArmorDetector.hpp 用于装甲板识别，必须有
- Camera.h 用于相机的配置、开启、关闭
- config.h 配置文件，必须有
- DxImageProc.h 