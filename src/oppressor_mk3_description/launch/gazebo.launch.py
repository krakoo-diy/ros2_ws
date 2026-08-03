import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, TimerAction, ExecuteProcess, SetEnvironmentVariable
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
import xacro
from os.path import join

def generate_launch_description():

    pkg_ros_gz_sim = get_package_share_directory('ros_gz_sim')
    pkg_ros_gz_rbot = get_package_share_directory('oppressor_mk3_description')


    robot_description_file = os.path.join(pkg_ros_gz_rbot, 'urdf', 'oppressor_mk3.xacro')
    ros_gz_bridge_config = os.path.join(pkg_ros_gz_rbot, 'config', 'ros_gz_bridge_gazebo.yaml')
    
    robot_description_config = xacro.process_file(robot_description_file)
    robot_description = {'robot_description': robot_description_config.toxml()}

    rviz_config_file = os.path.join(pkg_ros_gz_rbot, 'config', 'gazebo.rviz')

    gz_model_path = os.path.join(pkg_ros_gz_rbot, 'models')


    set_gz_resource_path = SetEnvironmentVariable(
        name='GZ_SIM_RESOURCE_PATH',
        value=gz_model_path + os.pathsep + os.environ.get('GZ_SIM_RESOURCE_PATH', '')
    )


   
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[robot_description],
    )


   
    gazebo = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(join(pkg_ros_gz_sim, "launch", "gz_sim.launch.py")),
        launch_arguments={"gz_args": "-r -v 4 " + os.path.join(get_package_share_directory('oppressor_mk3_description'), 'worlds', 'custom_world.sdf')}.items()
    )

    spawn_robot = TimerAction(
        period=5.0,  
        actions=[Node(
            package='ros_gz_sim',
            executable='create',
            arguments=[
                "-topic", "/robot_description",
                "-name", "oppressor_mk3",
                "-allow_renaming", "false",  # prevents "_1" duplicate
                "-x", "0.0",
                "-y", "-2.5",
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


    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster"],
    )

    diff_drive_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["diff_drive_controller"],
    )

    spawn_controllers = TimerAction(
        period=8.0,
        actions=[
            joint_state_broadcaster_spawner,
            diff_drive_spawner
        ]
    )

    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', rviz_config_file],
        output='screen'
    )

    image_processing = ExecuteProcess(
        cmd=['ros2', 'run', 'oppressor_api', 'image_processing'],
        output='screen',
        shell=True
    )
    camera_viewer = ExecuteProcess(
        cmd = ['ros2', 'run', 'rqt_image_view', 'rqt_image_view'],
        output = 'screen',
        shell = True
    )


    return LaunchDescription([
        set_gz_resource_path,
        robot_state_publisher,
        gazebo,
        spawn_robot,
        ros_gz_bridge,
        spawn_controllers,
        rviz_node,
        image_processing,
        camera_viewer


    ])
