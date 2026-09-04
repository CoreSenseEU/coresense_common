import os

import lifecycle_msgs
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (
    IncludeLaunchDescription,
    DeclareLaunchArgument,
    EmitEvent,
    RegisterEventHandler,
)
from launch.events import matches_action
from launch.substitutions import (
    LaunchConfiguration,
    PathJoinSubstitution,
)
from launch_ros.actions import LifecycleNode, Node
from launch_ros.event_handlers import OnStateTransition
from launch_ros.events.lifecycle import ChangeState
from launch_ros.substitutions import (
    FindPackageShare,
)


def generate_launch_description():
    
    kb_bringup_package_arg = DeclareLaunchArgument(
        'kb_bringup_package',
        default_value='coresense_bringup',
        description='Name of custom TriplestarKB bringup package',
    )
    bt_config_arg = DeclareLaunchArgument(
        'bt_config_file',
        default_value=[PathJoinSubstitution([FindPackageShare('coresense_bringup'), 'config', 'bt_controller_params.yaml'])],
        description='Configuration of plugins and behavior trees',
    )

    kb_launch = IncludeLaunchDescription(
        PathJoinSubstitution([FindPackageShare('coresense_bringup'), 'launch', 'kb.launch.py']),
        launch_arguments={'bringup_package': LaunchConfiguration('kb_bringup_package')}.items()
    )
    bt_controller_launch = IncludeLaunchDescription(
        PathJoinSubstitution([FindPackageShare('coresense_bringup'), 'launch', 'bt_controller.launch.py']),
        launch_arguments={'bt_config_file': LaunchConfiguration('bt_config_file')}.items()
    )
    return LaunchDescription(
        [
            kb_bringup_package_arg,
            bt_config_arg,
            kb_launch,
            bt_controller_launch
        ]
    )
