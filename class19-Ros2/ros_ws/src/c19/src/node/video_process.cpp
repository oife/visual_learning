#include "rclcpp/rclcpp.hpp"
//#include "std_msgs/msg/string.hpp"
//#include "video_process-opencv.h"
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
class VideoProcess:public rclcpp::Node
{
private:
    /* data */
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr vedio_process_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr hist_publisher_;
    void image_callback(const sensor_msgs::msg::Image::SharedPtr msg)
    {
        cv_bridge::CvImagePtr cv_ptr;
        try {
          
            cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    
        } catch (cv_bridge::Exception& error) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge 异常: %s", error.what());
            return;
        }

        cv::Mat image = cv_ptr->image;
            RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 1000,
                "收到图像，尺寸: %d x %d", 
                image.cols, 
                image.rows
            );
        //TODO:
        //处理图像生成直方图
        cv::Mat gray;
        cv::cvtColor(image,gray,cv::COLOR_BGR2GRAY);

        int histSize = 256;         // 0到255，共256个灰度级
        float range[] = {0, 256};   // 像素值的范围
        const float* histRange = {range};
    
        cv::Mat hist;
        cv::calcHist(&gray, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);
        //直方图的可视化
        int hist_w = 512, hist_h = 400;
        int bin_w = cvRound((double)hist_w / histSize);
        cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

        // 标准化直方图（将值缩放到 0 到 histImage.rows 之间）
        cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

        // 绘制每个 bin 的垂直线
        for (int i = 1; i < histSize; i++) {
            cv::line(histImage, 
            cv::Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
            cv::Scalar(255, 255, 255), 2, 8, 0);
        }

        cv_bridge::CvImage hist_cv_image;
        hist_cv_image.header = msg->header;
        hist_cv_image.encoding = sensor_msgs::image_encodings::BGR8;
        hist_cv_image.image = histImage;
        sensor_msgs::msg::Image::SharedPtr hist_msg = hist_cv_image.toImageMsg();
        hist_publisher_->publish(*hist_msg);
        RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "已发布直方图图像");
    }
public:

    VideoProcess(std::string name) : Node(name){

        RCLCPP_INFO(this->get_logger(),"%s节点已经启动",name.c_str());
        vedio_process_ = this->create_subscription<sensor_msgs::msg::Image>(
            "image_topic",
            10,
            std::bind(&VideoProcess::image_callback, this, std::placeholders::_1));
        hist_publisher_ = this->create_publisher<sensor_msgs::msg::Image>("histogram_topic", 10);
    }
};

int main(int argc, char **argv){
    rclcpp::init(argc,argv);

    auto node = std::make_shared<VideoProcess>("video_process");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;

}