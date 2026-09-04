import os

import lifecycle_msgs
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    EmitEvent,
    RegisterEventHandler,
)
from launch.events import matches_action
from launch.substitutions import (
    PathJoinSubstitution,
    LaunchConfiguration,
)
from launch_ros.actions import LifecycleNode, Node
from launch_ros.event_handlers import OnStateTransition
from launch_ros.events.lifecycle import ChangeState
from launch_ros.substitutions import (
    FindPackageShare,
)


def generate_launch_description():
    log_level_arg = DeclareLaunchArgument(
        'log-level',
        default_value='info',
        description='Logging level',
    )
    log_level = LaunchConfiguration('log-level', default='info')
 
    config_arg = DeclareLaunchArgument(
        'bt_config_file',
        default_value=[PathJoinSubstitution([FindPackageShare('coresense_bringup'), 'config', 'bt_controller_params.yaml'])],
        description='Configuration of plugins and behavior trees',
    )

    bt_controller_node = Node(
        package='coresense_bt_controller',
        executable='bt_controller',
        parameters=[LaunchConfiguration('bt_config_file')],
        name='bt_controller',
        namespace='',
        output='screen',
    )
    return LaunchDescription(
        [
            log_level_arg,
            config_arg,
            bt_controller_node
        ]
    )
