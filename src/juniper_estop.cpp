#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "juniper_board_msgs/srv/bool_estop.hpp" 

using namespace std::chrono_literals;

class Estop : public rclcpp::Node
{
public:
    Estop()
        : Node("juniper_estop"), estop_enabled_(true)
    {
        // Publisher for velocity commands
        vel_out_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
        
        // Estop status publisher
        estop_status_pub_ = this->create_publisher<std_msgs::msg::Bool>("/estop_status", 10);

        // Don't need a timer since going to publish whenever we get a message in subscription
        // timer_ = this->create_wall_timer(
        //     500ms, std::bind(&Estop::timer_callback, this));

        // Subscriber for incoming velocity commands
        vel_in_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel_regulated", 10, std::bind(&Estop::cmd_callback, this, std::placeholders::_1));

        // Service for toggling E-STOP
        toggle_srv_ = this->create_service<juniper_board_msgs::srv::BoolEstop>(
            "/toggle_estop",
            std::bind(&Estop::toggle_estop_callback, this, std::placeholders::_1, std::placeholders::_2));
    }

private:
    // Don't need a timer since going to publish whenever we get a message in subscription
    // void timer_callback()
    // {
    // }

    void toggle_estop_callback(
        const std::shared_ptr<juniper_board_msgs::srv::BoolEstop::Request> request,
        std::shared_ptr<juniper_board_msgs::srv::BoolEstop::Response> response)
    {
        // Toggle E-STOP
        estop_enabled_ = request->estop;
        response->success = true;
        if (estop_enabled_)
        {
            response->message = "E-STOP Enabled";
            RCLCPP_INFO(this->get_logger(), "E-STOP Enabled, robot will not move");
        }
        else
        {
            response->message = "E-STOP Disabled";
            RCLCPP_INFO(this->get_logger(), "E-STOP Disabled, robot is able to move");
        }
        
    }

    void cmd_callback(const geometry_msgs::msg::Twist::SharedPtr msg)
    {
        auto status_msg = std_msgs::msg::Bool();

        if (estop_enabled_)
        {
            // Send 0's if estop is enabled
            auto zero_vel_msg = geometry_msgs::msg::Twist();
            zero_vel_msg.linear.x = 0.0;
            zero_vel_msg.angular.z = 0.0;
            vel_out_pub_->publish(zero_vel_msg);

           
            status_msg.data=1;
            estop_status_pub_->publish(status_msg);
            return;
        }
        else
        {
            // Republish estop command if estop disabled
            vel_out_pub_->publish(*msg);
            status_msg.data=0;
            estop_status_pub_->publish(status_msg);
        }    
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr vel_out_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr estop_status_pub_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr vel_in_sub_;
    rclcpp::Service<juniper_board_msgs::srv::BoolEstop>::SharedPtr toggle_srv_;

    bool estop_enabled_; 
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Estop>());
    rclcpp::shutdown();
    return 0;
}
