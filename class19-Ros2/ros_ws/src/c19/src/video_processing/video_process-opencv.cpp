#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap("../video_source/vedio.mp4");
    
    if (!cap.isOpened()) {
        cout << "无法打开输入视频文件" << endl;
        return -1;
    }

    int frameWidth = cap.get(CAP_PROP_FRAME_WIDTH);
    int frameHeight = cap.get(CAP_PROP_FRAME_HEIGHT);
    double fps = cap.get(CAP_PROP_FPS);

    int delay = 1000/fps;

    Mat frame;
    while (true)
    {
        cap.read(frame);

        if (frame.empty()) {
            break;
        }
        imshow("video", frame);

        char c = waitKey(delay);
        if (c == 27) { //ord(ESC)
            break;
        }
    }

    cap.release();
    return 0;
}