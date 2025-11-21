import cv2

cap = cv2.VideoCapture("../src/vedio.mp4")
#cap = cv2.VideoCapture(0)
#cap = cv2.VideoCapture("./imwrite/video_30frames.mp4")

if not cap.isOpened():
    print("无法打开视频文件或摄像头")
    exit()

fps = cap.get(cv2.CAP_PROP_FPS)
delay = int(1000/fps)
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

#print("the vedio's size is :",frame_width,frame_height)

fourcc = cv2.VideoWriter_fourcc(*'mp4v')
out = cv2.VideoWriter("./imwrite/video_30frames.mp4",fourcc,fps,(frame_width,frame_height))

if not out.isOpened():
    print("未能创建输出视频文件\n")
    print("继续播放视频")

paused = False
ret,frame = cap.read()
out.write(frame)

frame_trace=1

if not ret:
    print("video error")
    cap.release()
    exit()

while True:
    
    cv2.imshow("vedio_play",frame)

    c=cv2.waitKey(0 if paused else delay) & 0xff
    if c == 27:#27 is the ascii code for ESC
        break
    elif c == 0x71:#ord(q)
        break
    elif c == 0x20 :#ord(Space)
        paused = not paused
    if(not paused):
        ret,frame = cap.read()
        if not ret:
            print("the vedio is over")
            break
        if(frame_trace<=30):
            out.write(frame)
            frame_trace+=1


cap.release()
out.release()
cv2.destroyAllWindows()