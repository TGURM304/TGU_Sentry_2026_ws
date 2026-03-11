#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>

class IMURotateNode : public rclcpp::Node
{
public:
    IMURotateNode()
        : Node("imu_rotate_node", rclcpp::NodeOptions().use_intra_process_comms(true))
    {
        publisher_ = this->create_publisher<sensor_msgs::msg::Imu>("imu/data", 10);
        subscription_ = this->create_subscription<sensor_msgs::msg::Imu>(
            "livox/imu", 10,
            std::bind(&IMURotateNode::listener_callback, this, std::placeholders::_1));

        // 你的旋转矩阵（雷达 → IMU）
        R_ = tf2::Matrix3x3(
            0.6691,  0.0,   0.7431,
            0.0,     1.0,   0.0,
           -0.7431,  0.0,   0.6691
        );
    }

private:
    void listener_callback(const sensor_msgs::msg::Imu::UniquePtr msg)
    {
        sensor_msgs::msg::Imu out = *msg;

        /* ===== 1. 旋转角速度 ===== */
        tf2::Vector3 w(
            msg->angular_velocity.x,
            msg->angular_velocity.y,
            msg->angular_velocity.z
        );
        w = R_ * w;

        out.angular_velocity.x = w.x();
        out.angular_velocity.y = w.y();
        out.angular_velocity.z = w.z();

        /* ===== 2. 旋转线加速度 ===== */
        tf2::Vector3 a(
            msg->linear_acceleration.x,
            msg->linear_acceleration.y,
            msg->linear_acceleration.z
        );
        a = R_ * a;

        out.linear_acceleration.x = a.x();
        out.linear_acceleration.y = a.y();
        out.linear_acceleration.z = a.z();

        /* ===== 3. 旋转姿态四元数 ===== */
        tf2::Quaternion q(
            msg->orientation.x,
            msg->orientation.y,
            msg->orientation.z,
            msg->orientation.w
        );

        tf2::Quaternion q_R;
        R_.getRotation(q_R);

        tf2::Quaternion q_out = q_R * q;
        q_out.normalize();

        out.orientation.x = q_out.x();
        out.orientation.y = q_out.y();
        out.orientation.z = q_out.z();
        out.orientation.w = q_out.w();

        publisher_->publish(out);
    }

    rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr publisher_;
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr subscription_;

    tf2::Matrix3x3 R_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<IMURotateNode>());
    rclcpp::shutdown();
    return 0;
}
