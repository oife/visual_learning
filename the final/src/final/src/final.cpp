#include "rclcpp/rclcpp.hpp"
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

#include "aim_interfaces/msg/aim_info.hpp"

//#include "final.h"//ros有自带创建接口功能

class Final:public rclcpp::Node
{
private:
    //一些函数
    double deg_to_rad(double deg) {
        return deg * M_PI / 180.0;
    }
    /* data */
    //相机内参
    const cv::Mat CAMERA_MATRIX = (cv::Mat_<double>(3, 3)<<
        1462.3697,          0, 398.59394,
                0, 1469.68385, 110.68997,
                0,          0,        1
        );
    //畸变矩阵
    const cv::Mat DIST_COEFFS = (cv::Mat_<double>(1,5)<<
            0.003518, -0.311778,-0.016581,0.023682,0.0000
        );
    //数字装甲板两侧灯条间距16cm，长度8cm
    const double HALF_WIDTH = 0.16 / 2.0; // 0.08m
    const double HALF_LENGTH = 0.08 / 2.0; // 0.04m
    const std::vector<cv::Point3f> ARMOR_3D_POINTS = {
        // 顺序必须与 PnP 输入的 2D 角点顺序一致：右下, 右上, 左上, 左下
        cv::Point3f( HALF_WIDTH, -HALF_LENGTH, 0),
        cv::Point3f( HALF_WIDTH,  HALF_LENGTH, 0),
        cv::Point3f(-HALF_WIDTH,  HALF_LENGTH, 0),
        cv::Point3f(-HALF_WIDTH, -HALF_LENGTH, 0)
    }; 
    //---------------------------------------------------------
    //==========================================================
    //此时相机相对于机器人坐标系的一个偏移为(x, y, z) = 
    //(0.08, 0.0, 0.05)（单位：m），
    //(roll, pitch, yaw) = (0, 60, 20)（角度制）。
    //===========================================================
    const double ROLL = deg_to_rad(0.0);
    const double PITCH = deg_to_rad(60.0);
    const double YAW = deg_to_rad(20.0);
    cv::Mat R_x = (cv::Mat_<double>(3, 3) <<
        1,       0,        0,
        0, cos(ROLL), -sin(ROLL),
        0, sin(ROLL),  cos(ROLL)
    );

    cv::Mat R_y = (cv::Mat_<double>(3, 3) <<
        cos(PITCH),  0, sin(PITCH),
        0,           1,          0,
        -sin(PITCH), 0, cos(PITCH)
    );

    cv::Mat R_z = (cv::Mat_<double>(3, 3) <<
        cos(YAW), -sin(YAW), 0,
        sin(YAW),  cos(YAW), 0,
        0,         0,        1
    );

    cv::Mat R_rc = R_z * R_y * R_x;//旋转矩阵,robot to camera
    cv::Mat t_rc  = (cv::Mat_<double>(3, 1) <<//平移向量
            0.08, 
            0.0, 
            0.05
        );
//================================================================
//常数和已知量定义完成
//================================================================


    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr input_;
    rclcpp::Publisher<aim_interfaces::msg::AimInfo>::SharedPtr output_;
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
        //处理图像
        aim_interfaces::msg::AimInfo result_info;
        if (process_image(image, result_info)) {
            output_->publish(result_info);
        }
    }
    struct LightBar {
    cv::RotatedRect rect;
    cv::Point2f center;
    float length;
    float width;
    float angle;
    };
    bool process_image(cv::Mat frame,aim_interfaces::msg::AimInfo &result_info){
        cv::Mat binary_img;
        // cv::Mat b_channel, g_channel, r_channel;
        // std::vector<cv::Mat> channels;
        // cv::split(frame, channels);
        // b_channel = channels[0];
        // g_channel = channels[1];
        // r_channel = channels[2];

        // // 蓝色灯条：B 通道减去 R 通道，结果图像中蓝色区域亮度高
        // cv::Mat sub_img = b_channel - r_channel;
        
        // 二值化
        //cv::threshold(sub_img, binary_img, 20, 255, cv::THRESH_BINARY);
        cv::Mat gray_image;
        cv::cvtColor(frame,gray_image,cv::COLOR_BGR2GRAY);
        cv::threshold(gray_image, binary_img, 240, 255, cv::THRESH_BINARY);
        // 形态学操作
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::erode(binary_img,binary_img,kernel);
        cv::dilate(binary_img, binary_img, kernel);
        cv::dilate(binary_img, binary_img, kernel);
        
        //灯条匹配
        std::vector<LightBar> light_bars;//合格灯条存储数组
        std::vector<std::vector<cv::Point>> contours;//轮廓
        cv::findContours(binary_img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        for (const auto& contour : contours) {
            double area = cv::contourArea(contour);
            
            // 1. 面积筛选：排除过小（噪声）或过大（非灯条）的轮廓
            if (area < 20 || area > 5000) continue; 

            // 2. 拟合最小外接矩形，获取灯条几何信息
            cv::RotatedRect rotated_rect = cv::minAreaRect(contour);
            
            float len = rotated_rect.size.height;
            float wid = rotated_rect.size.width;
            // 确保 len 始终为长边，wid 始终为短边
            if (wid > len) std::swap(len, wid);
            
            // 3. 长宽比筛选：灯条的长宽比通常较高（2.0 ~ 10.0）
            float ratio = len / wid;
            if (ratio < 1.0 || ratio > 10.0) continue; 
            
            // 4. 角度筛选：灯条应该是相对垂直的（排除倾斜过大的）
            float angle = rotated_rect.angle;
            if (angle < -45.0) angle += 90.0; // 归一化角度
            if (std::abs(angle) > 25) continue; // 倾斜超过 25 度则丢弃

            // 存储合格的灯条
            light_bars.push_back({rotated_rect, rotated_rect.center, len, wid, angle});
            RCLCPP_INFO(this->get_logger(), "合格灯条 [Length: %.2f, Width: %.2f, Angle: %.2f]", len, wid, angle);
        }

        if (light_bars.size() < 2) {//合格灯条数量
            RCLCPP_INFO(this->get_logger(), "Not enough light bars found.");
            //return false;
        }
        RCLCPP_INFO(this->get_logger(), "lightbars' size is %d",light_bars.size());
        std::vector<cv::Point2f> final_armor_2d_points;
        int target_type = 0; 
        bool armor_found = false;
        for (size_t i = 0; i < light_bars.size(); ++i) {
                for (size_t j = i + 1; j < light_bars.size(); ++j) {
                    
                    // 确保 l1 在左，l2 在右
                    const LightBar *pl1 = &light_bars[i];
                    const LightBar *pl2 = &light_bars[j];
                    if (pl1->center.x > pl2->center.x) std::swap(pl1, pl2);
                    const auto& l1 = *pl1;
                    const auto& l2 = *pl2;

                    float center_dist = cv::norm(l1.center - l2.center);
                    float avg_length = (l1.length + l2.length) / 2.0;

                    // 1. 间距约束：间距/平均长度比（例如：1.0 到 4.0）
                    // float dist_ratio = center_dist / avg_length;
                    // if (dist_ratio < 1.0 || dist_ratio > 4.0) {
                    //     RCLCPP_INFO(this->get_logger(), "Match Failed: Dist Ratio (%.2f)", dist_ratio);
                    //     continue;
                    // }

                    float length_ratio = std::max(l1.length, l2.length) / std::min(l1.length, l2.length);
                    if (length_ratio > 2.5) {
                        RCLCPP_INFO(this->get_logger(), "Match Failed: Length Ratio (%.2f)", length_ratio);
                        continue;
                    }

                    float angle_diff = std::abs(l1.angle - l2.angle);
                    if (angle_diff > 10.0) {
                        RCLCPP_INFO(this->get_logger(), "Match Failed: Angle Diff (%.2f)", angle_diff);
                        continue;
                    }

                    float y_diff = std::abs(l1.center.y - l2.center.y);
                    if (y_diff / avg_length > 0.3) {
                        RCLCPP_INFO(this->get_logger(), "Match Failed: Y Diff Ratio (%.2f)", y_diff / avg_length);
                        continue;
                    }

                    // 成功日志
                    RCLCPP_INFO(this->get_logger(), "Match SUCCEEDED! Extracting 2D points.");
                    // ... (后面的 2D 点提取和 push_back 逻辑)

                    // --- 匹配成功：提取精确的 4 个角点 ---
                    cv::Point2f pts1[4], pts2[4];
                    l1.rect.points(pts1); // 左灯条的四个角点
                    l2.rect.points(pts2); // 右灯条的四个角点
                    
                    // 目标：按照 ARMOR_3D_POINTS 的顺序 (右下, 右上, 左上, 左下) 提取 2D 角点
                    // 需要找到每个旋转矩形中 y 坐标最小/最大，且靠近/远离中心点的角点。
                    
                    // 简化处理：找到右灯条（l2）的上下端点和左灯条（l1）的上下端点
                    // 找到右灯条的y坐标最大（下）和最小（上）的角点
                    cv::Point2f r_top = l2.center, r_bottom = l2.center;
                    for(int k=0; k<4; k++) {
                        if (pts2[k].y < r_top.y) r_top = pts2[k];
                        if (pts2[k].y > r_bottom.y) r_bottom = pts2[k];
                    }
                    // 找到左灯条的y坐标最大（下）和最小（上）的角点
                    cv::Point2f l_top = l1.center, l_bottom = l1.center;
                    for(int k=0; k<4; k++) {
                        if (pts1[k].y < l_top.y) l_top = pts1[k];
                        if (pts1[k].y > l_bottom.y) l_bottom = pts1[k];
                    }
                    
                    // 最终 2D 角点顺序：右下, 右上, 左上, 左下
                    //final_armor_2d_points = {r_bottom, r_top, l_top, l_bottom};
                    final_armor_2d_points.push_back(r_bottom);
                    final_armor_2d_points.push_back(r_top);
                    final_armor_2d_points.push_back(l_top);
                    final_armor_2d_points.push_back(l_bottom);
                    // -----------------------------------------------------------
                    // --- 图案识别 ---
                    // -----------------------------------------------------------
                    // ================================
                    //TODO:图像识别
                    //==================================
                    target_type = 7; 
                    
                    armor_found = true;
                    break; // 找到一个装甲板即跳出内层循环
                }
                if (armor_found) break; // 跳出外层循环
            }
            
            if (!armor_found) {
                RCLCPP_INFO(this->get_logger(), "No valid armor plate found after matching.");
                //return false;
            }
            // 关键修正：在调用 PnP 前，确保点集大小为 4 
            if (ARMOR_3D_POINTS.size() != 4 || final_armor_2d_points.size() != 4) {
                RCLCPP_ERROR(this->get_logger(), "PnP input point count error: 3D points size: %zu, 2D points size: %zu.",
                    ARMOR_3D_POINTS.size(), final_armor_2d_points.size());
                return false;
            }
            //----------pnp求解--------------------
            cv::Mat rvec_cam, tvec_cam; 
            try {
                cv::solvePnP(ARMOR_3D_POINTS, final_armor_2d_points, CAMERA_MATRIX, DIST_COEFFS, 
                            rvec_cam, tvec_cam, false, cv::SOLVEPNP_ITERATIVE);
            } catch (const cv::Exception& e) {
                RCLCPP_ERROR(this->get_logger(), "PnP solve failed: %s", e.what());
                //return false;
            }
            //坐标系转换
            cv::Mat R_cr = R_rc.t(); // 相机到机器人的旋转矩阵 (R_robot_to_camera 的转置)
            cv::Mat t_cr = -R_cr * t_rc; // 相机到机器人的平移向量 (利用公式 t_cr = -R_cr * t_rc)
            
            // 2. 坐标转换公式: T_target_robot = R_cr * T_target_cam + t_cr
            // tvec_cam 是目标在相机坐标系下的平移向量 (T_target_cam)
            cv::Mat target_robot_pos = R_cr * tvec_cam + t_cr;
            result_info.coordinate.push_back(static_cast<int16_t>(std::round(target_robot_pos.at<double>(0) * 1000.0)));
            result_info.coordinate.push_back(static_cast<int16_t>(std::round(target_robot_pos.at<double>(1) * 1000.0)));
            result_info.coordinate.push_back(static_cast<int16_t>(std::round(target_robot_pos.at<double>(2) * 1000.0)));
            
            // 2. 类型 (Int16 type)
            result_info.type = static_cast<int16_t>(target_type);

            RCLCPP_INFO(this->get_logger(), 
                "Target Found! Coords: [%d, %d, %d] mm, Type: %d",
                result_info.coordinate[0], result_info.coordinate[1], result_info.coordinate[2],
                result_info.type);

            return true;

    }
public:
    Final(std::string name) : Node(name){
        RCLCPP_INFO(this->get_logger(),"%s节点已经启动",name.c_str());
        input_ = this->create_subscription<sensor_msgs::msg::Image>(
            "sensor_img",
            10,
            std::bind(&Final::image_callback, this, std::placeholders::_1));
        output_ = this->create_publisher<aim_interfaces::msg::AimInfo>("aim_target", 10);
    }
};

int main(int argc, char **argv){
    rclcpp::init(argc,argv);

    auto node = std::make_shared<Final>("final");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;

}