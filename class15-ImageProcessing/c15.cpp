#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(){
    VideoCapture cap("../src/greenSrc.mp4");
    if(!cap.isOpened()){
        cout<<"无法打开输入视频文件"<<endl;
        return -1;
    }

    double fps = cap.get(CAP_PROP_FPS);
    int delay = 1000/fps;//每帧显示时间

    Mat frame;
    
    while(true){
        cap.read(frame);
        if(frame.empty()){
            break;
        }
        //imshow("视频播放",frame);

        char c = waitKey(delay) & 0xff;
        if(c==27){//ord(ESC)=27
            break;
        }
    }
    cout<<"1234556"<<endl;
}