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
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    cv::Mat frame;
    void timer_callback(){

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
    VideoRead(std::string name,cv::Mat frame) : Node(name),frame(frame){
        RCLCPP_INFO(this->get_logger(),"%s节点已经启动",name.c_str());
        image_publisher_ = this->create_publisher<sensor_msgs::msg::Image>("sensor_img",10);

        timer_ = this->create_wall_timer(std::chrono::milliseconds(1500),std::bind(&VideoRead::timer_callback,this));
    }
};

int main(int argc, char **argv){
    std::string package_name = "final"; 
    std::string package_share_directory = 
    ament_index_cpp::get_package_share_directory(package_name);

    cv::Mat image;

    image = cv::imread(package_share_directory+"/srcimmage/image.png");

    rclcpp::init(argc,argv);
    auto node = std::make_shared<VideoRead>("Publisher",image);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}