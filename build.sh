source /opt/ros/humble/setup.sh
./src/livox_ros_driver2/build.sh humble
source /home/tgu/Desktop/tgu_sentry_2026_ws/install/setup.bash
colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release --parallel-workers 3
