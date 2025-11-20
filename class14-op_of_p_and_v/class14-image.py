#图像操作
import cv2
import re
import os

os.environ['QT_QPA_PLATFORM'] = 'xcb'#解决qt的wayland报错，切换为xcb

image_address = "../src/opencv_logo.jpg"

image = cv2.imread("../src/opencv_logo.jpg")


if image is None:
    print("图片无法打开")
    exit(0)

cv2.imshow("image_show", image)#名字不能输中文 
cv2.waitKey()

if(re.match(".*\\.jpg",image_address)):
    to_png_success = cv2.imwrite("./imwrite/converted_image.png",image)
if(re.match(".*\\.png",image_address)):
    to_jpg_success = cv2.imwrite("./imwrite/converted_image.jpg",image)





