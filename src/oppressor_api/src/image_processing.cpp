

#include <chrono>
#include <functional>
#include <memory>

#include "geometry_msgs/msg/twist_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/camera_info.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/float64.hpp"
#include <string>

#include "cv_bridge/cv_bridge.hpp"
#include "opencv2/opencv.hpp"

class ImageProcessor : public rclcpp::Node {
public:
  ImageProcessor() : Node("image_processor") {
    image_subscriber_ = create_subscription<sensor_msgs::msg::Image>(
        "/camera/front/image", 10,
        [this](sensor_msgs::msg::Image::SharedPtr image) {
          error_callback(image);
        });
    error_publisher_ = create_publisher<std_msgs::msg::Float64>("/error", 10);
  }

private:
  void error_callback(sensor_msgs::msg::Image::SharedPtr image) {
    // creating an image copy with an encoding data type of BGR8 regarding any
    // image type
    cv_bridge::CvImagePtr cv_ptr;
    try {
      cv_ptr = cv_bridge::toCvCopy(image, sensor_msgs::image_encodings::BGR8);
      // if any error is detected it will be printed out
    } catch (cv_bridge::Exception &e) {
      RCLCPP_ERROR(get_logger(), "CV bridge exeption: %s", e.what());
      return;
    }
    cv::Mat frame = cv_ptr->image;

    // frame corping to 1/3 of frame
    int roi_height = frame.rows / 3;
    cv::Rect roi(0, frame.rows - roi_height, frame.cols,
                 roi_height); // opencv roi function creates a new part out of
                              // copyed frame
    // roi(x, y, width, height) frame.rows = total hight /
    cv::Mat cropped = frame(roi); // Image corp, the ROI is the bottom part

    // converting into grayscale color filtring
    cv::Mat grey;
    cv::cvtColor(cropped, grey, cv::COLOR_BGR2GRAY);
    // by converting to BGR8 image into two colors only it will make easy for us
    // to detect the deired color
    cv::Mat binary;
    cv::threshold(grey, binary, 60, 255, cv::THRESH_BINARY_INV);

    // contours, finding or detecting the objects based on a specifc value
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);

    if (contours.empty()) {
      // no line detected — handle separately, see below
    } else {
      double max_area = 0;
      int max_index = -1;
      for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area > max_area) {
          max_area = area;
          max_index = static_cast<int>(i);
        }
      }

      cv::Moments M = cv::moments(contours[max_index]);
      double centroid_x = M.m10 / M.m00;
      double image_center_x = binary.cols / 2.0;
      double error = centroid_x - image_center_x;

      auto msg = std_msgs::msg::Float64();
      msg.data = error;
      // RCLCPP_INFO(get_logger(), "Error value in pixels: %f", msg.data);
      error_publisher_->publish(msg);
    }
  }
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_subscriber_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr error_publisher_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto processing_node = std::make_shared<ImageProcessor>();
  rclcpp::spin(processing_node);
  rclcpp::shutdown();
  return 0;
}