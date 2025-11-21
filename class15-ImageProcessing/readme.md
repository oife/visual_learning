# Class 15: opencv 基础图像处理 抠图
使用python完成
c15-gray.py基于阈值分割法，使用二值化阈值[^1],效果不好：眼睛亮度与背景混合
c14-hsv.py同样基于阈值分割法，但在HSV颜色空间[^2]

[^1]:mask通过cv2.threshold()/二值化函数|获得
[^2]:mask通过cv2.inRange()获得
