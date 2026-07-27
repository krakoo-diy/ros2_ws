import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, TimerAction, ExecuteProcess
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from launch.substitutions import PathJoinSubstitution
from launch.substitutions import LaunchConfiguration, Command
from launch_ros.substitutions import FindPackageShare



import xacro
from os.path import join

def generate_launch_description():

    pkg_ros_gz_sim = get_package_share_directory('ros_gz_sim')
    pkg_ros_gz_rbot = get_package_share_directory('r_arm_description')


    robot_description_file = os.path.join(pkg_ros_gz_rbot, 'urdf', 'r_arm.xacro')
    ros_gz_bridge_config = os.path.join(pkg_ros_gz_rbot, 'config', 'ros_gz_bridge_gazebo.yaml')
    
    robot_description_config = xacro.process_file(robot_description_file)
    robot_description = {'robot_description': robot_description_config.toxml()}

    rviz_config_file = os.path.join(pkg_ros_gz_rbot, 'config', 'gazebo.rviz')

   
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='both',
        parameters=[robot_description],
    )
    joint_state_publisher_node = Node(
        package='joint_state_publisher',
        executable='joint_state_publisher',
        name='joint_state_publisher',
    )
   
    gazebo = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(join(pkg_ros_gz_sim, "launch", "gz_sim.launch.py")),
        launch_arguments={"gz_args": "-r -v 4 empty.sdf"}.items()
    )

    spawn_robot = TimerAction(
        period=5.0,  
        actions=[Node(
            package='ros_gz_sim',
            executable='create',
            arguments=[
                "-topic", "/robot_description",
                "-name", "r_arm",
                "-allow_renaming", "false",  # prevents "_1" duplicate
                "-x", "0.0",
                "-y", "0.0",
                "-z", "0.32",
                "-Y", "0.0"
            ],
            output='screen'
        )]
    )

    ros_gz_bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        parameters=[{'config_file': ros_gz_bridge_config}],
        output='screen'
    )
    
    load_joint_state_broadcaster = ExecuteProcess(
        cmd=['ros2', 'control', 'load_controller', '--set-state', 'active', 'joint_state_broadcaster'],
        output='screen'
    )

    load_position_controller = ExecuteProcess(
        cmd=['ros2', 'control', 'load_controller', '--set-state', 'active', 'position_control'],
        output='screen'
    )

    load_velocity_controller = ExecuteProcess(
        cmd=['ros2', 'control', 'load_controller', '--set-state', 'inactive', 'velocity_control'],
        output='screen'
    )

    load_sinu_controller = ExecuteProcess(
        cmd=['ros2', 'control', 'load_controller', '--set-state', 'inactive', 'sine_controller'],
        output='screen'
    )
    
    rviz_node = Node(
        package ='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', rviz_config_file],
        output='screen'
    )

    

    return LaunchDescription([
        gazebo,
        spawn_robot,
        ros_gz_bridge,
        robot_state_publisher,
        joint_state_publisher_node,
        load_joint_state_broadcaster,
        load_position_controller,
        load_velocity_controller,
        load_sinu_controller,
        rviz_node,
        
    ])
