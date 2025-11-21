import cv2
import numpy as np

# #src get

def extract():
    return 0    

cap = cv2.VideoCapture("../src/greenSrc.mp4")
# bg = np.zeros((1280, 720, 3), dtype=np.uint8)
bg = cv2.imread("../src/opencv_logo.jpg")

if not cap.isOpened():
    print("无法打开视频")
    exit()

fps = cap.get(cv2.CAP_PROP_FPS)
delay = int(1000/fps)#每帧显示时间ms
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
#print("the vedio's size is :",frame_width,frame_height)#1280x720
bg = cv2.resize(bg,(frame_width,frame_height))

paused = False

while True:
    if(not paused):
        ret,frame = cap.read()

        if not ret : 
            print("播放完毕")
            break
    
    hsv = cv2.cvtColor(frame,cv2.COLOR_BGR2HSV)
    lower_green = np.array([40, 50, 100])  
    upper_green = np.array([177, 255, 255]) 
    mask = cv2.inRange(hsv,lower_green,upper_green)#标记绿色
    mask = ~mask#反转掩码，让绿色变为背景
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(3,3))
    mask = cv2.morphologyEx(mask,cv2.MORPH_CLOSE,kernel,iterations=2)
    mask = cv2.morphologyEx(mask,cv2.MORPH_OPEN,kernel,iterations=1)
    foreground = cv2.bitwise_and(frame,frame,mask=mask)
    background = cv2.bitwise_and(bg,bg,mask=~mask)
    result = cv2.add(foreground,background)
    cv2.imshow("result",result)

    cv2.imshow("vedio show",frame)
    #cv2.imshow("test",mask)
    

    c = cv2.waitKey(0 if paused else delay) & 0xff

    if c == 27:#ord(esc)
        break#按esc退出
    if c == 32:#ord(space)
        paused = not paused
    

cap.release()
cv2.destroyAllWindows()
