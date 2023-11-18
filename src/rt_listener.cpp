// Devansh Agrawal
// Nov 2023

#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "builtin_interfaces/msg/duration.hpp"
#include "builtin_interfaces/msg/time.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;

class RTListener : public rclcpp::Node {

public:
  RTListener() : Node("rt_listener") {

    subscriber_ = this->create_subscription<builtin_interfaces::msg::Duration>(
        "round_trip", 10, std::bind(&RTListener::rt_callback, this, _1));

    timer_ = this-> create_wall_timer(
		    std::chrono::duration<double>(1.0),
		    std::bind(&RTListener::timer_callback, this));

    delay_ = this->declare_parameter<int>("delay_us", 0);
    print_ = this->declare_parameter<int>("print", 0);


  }

private:
  void rt_callback(const builtin_interfaces::msg::Duration &msg) {

    constexpr uint64_t s2ns = 1e9;

    double val = (double)msg.sec*(double)s2ns + (double)msg.nanosec;
    if (print_==1){
      RCLCPP_INFO(get_logger(), " ...avg round trip delay: %f ms", val);
    }
    if (print_==2){
      std::cout << " ...avg round trip delay: " << val << "\n";
    }

    avg = f * avg + (1-f) * val;

    usleep(delay_);

  }

  void timer_callback() {
    constexpr double ns2ms = 1e-6;

    RCLCPP_INFO(get_logger(), " ...avg round trip delay: %f ms", avg * ns2ms);
  }

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Subscription<builtin_interfaces::msg::Duration>::SharedPtr subscriber_;
    int delay_;
    double avg = 0;
    double f = 0.9;
    int print_ = 0;
};

int main(int argc, char **argv) {

  rclcpp::init(argc, argv);
  auto node = std::make_shared<RTListener>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
