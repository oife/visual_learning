import cv2

cap = cv2.VideoCapture(0, cv2.CAP_V4L2)

if not cap.isOpened():
    print("错误: 无法打开摄像头")
    exit()

cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

class state:
    is_original = 0
    is_gray = 1
    is_gauss =2

camera_state = state.is_original

while True:
    # 读取摄像头帧
    ret, frame = cap.read()
    
    if not ret:
        print("无法读取摄像头帧")
        break
    
    # 显示图像
    if(camera_state == state.is_original):
        cv2.putText(frame,"Original",(20,50),cv2.FONT_HERSHEY_SIMPLEX,2,(255,255,255),2)
        cv2.imshow('Camera', frame)
    if(camera_state == state.is_gray):
        gray = cv2.cvtColor(frame,cv2.COLOR_RGB2GRAY)
        cv2.putText(gray,"Gray",(20,50),cv2.FONT_HERSHEY_SIMPLEX,2,(255,255,255),2)
        cv2.imshow("Camera",gray)
    if(camera_state == state.is_gauss):
        gauss = cv2.GaussianBlur(frame,(0,0),6)
        cv2.putText(gauss,"GaussianBlur",(20,50),cv2.FONT_HERSHEY_SIMPLEX,2,(255,255,255),2)
        cv2.imshow("Camera",gauss)
    
    c = cv2.waitKey(1) & 0xFF

    # 按 'q' 键退出
    if c == ord('q'):
        break
    elif c == ord('b'):
        camera_state = state.is_gauss
    elif c == ord('g'):
        camera_state = state.is_gray
    elif c == ord('o'):
        camera_state = state.is_original

# 释放资源
cap.release()
cv2.destroyAllWindows()