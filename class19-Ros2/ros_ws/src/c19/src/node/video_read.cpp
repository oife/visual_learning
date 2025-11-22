#include "rclcpp/rclcpp.hpp"
//#include "std_msgs/msg/string.hpp"
//#include "video_process-opencv.h"
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>

class VideoRead:public rclcpp::Node
{
private:
    /* data */
    cv::VideoCapture cap;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    void timer_callback(){
        cv::Mat frame;
        cap.read(frame);

        if (frame.empty()) {
        RCLCPP_WARN(this->get_logger(), "视频读取完毕或失败");
        timer_->cancel();
        return; 
    }

        cv_bridge::CvImage cv_image;

        cv_image.image = frame;
        cv_image.header.stamp = this->now();
        cv_image.header.frame_id = "video_frame";
        cv_image.encoding = sensor_msgs::image_encodings::BGR8;
        
        sensor_msgs::msg::Image::SharedPtr msg = cv_image.toImageMsg();
        image_publisher_->publish(*msg);
    }

public:
    VideoRead(std::string name,cv::VideoCapture cap) : Node(name),cap(cap){
        RCLCPP_INFO(this->get_logger(),"%s节点已经启动",name.c_str());
        image_publisher_ = this->create_publisher<sensor_msgs::msg::Image>("image_topic",10);

        double fps = cap.get(cv::CAP_PROP_FPS);
        int delay = 1000/fps;
        if (delay <= 0) {
            RCLCPP_WARN(this->get_logger(), "FPS为零或无效,使用默认延迟 30ms (约33Hz)");
            delay = 30;
        }
        timer_ = this->create_wall_timer(std::chrono::milliseconds(delay),std::bind(&VideoRead::timer_callback,this));
    }
};

int main(int argc, char **argv){
    std::string package_name = "c19"; 
    std::string package_share_directory = 
    ament_index_cpp::get_package_share_directory(package_name);

    cv::VideoCapture cap(package_share_directory+"/video_source/vedio.mp4");

    rclcpp::init(argc,argv);
    auto node = std::make_shared<VideoRead>("video_read",cap);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;

}