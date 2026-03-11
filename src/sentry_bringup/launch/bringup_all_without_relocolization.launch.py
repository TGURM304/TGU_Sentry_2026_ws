import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource, FrontendLaunchDescriptionSource
from launch_ros.actions import Node


def generate_launch_description():
    bringup_dir = get_package_share_directory('sentry_bringup')

    # =======================
    # 1. FAST-LIO Mapping（核心）
    # =======================
    start_mapping = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(bringup_dir, 'launch', 'mapping.launch.py')
        )
    )

    # =======================
    # 2. Navigation (Nav2)
    # =======================
    start_navigation = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(bringup_dir, 'launch', 'navigation_noreloco.launch.py')
        )
    )

    # =======================
    # 3. 决策（可选）
    # =======================
    start_decision = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('rm_decision_cpp'),
                'launch',
                'run.launch.py'
            )
        )
    )

    # =======================
    # 4. 串口 / 底盘
    # =======================
    start_serial_driver = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('rm_serial_driver'),
                'launch',
                'serial_driver.launch.py'
            )
        )
    )

    # =======================
    # 5. 控制面板
    # =======================
    start_control_panel = Node(
        package='control_panel',
        executable='control_panel',
        output='screen'
    )

    # =======================
    # 6. 延时启动 Nav2（等 SLAM 稳定）
    # =======================
    delayed_start_navigation = TimerAction(
        period=8.0,   # 比你原来的 10s 略短，也可以保留 10s
        actions=[
            start_navigation,
            start_control_panel
        ]
    )

    # =======================
    # 7. 总 LaunchDescription
    # =======================
    ld = LaunchDescription()

    # 先启动 SLAM
    ld.add_action(start_mapping)

    # 再启动底盘
    ld.add_action(start_serial_driver)

    # 最后启动 Nav2
    ld.add_action(delayed_start_navigation)

    # ld.add_action(start_decision)  # 需要自动决策再打开

    return ld
