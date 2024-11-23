# `livox2pointcloud`

> Converts `livox_ros_driver2/CustomMsg` from official ROS livox driver to `sensor_msgs/PointCloud2` for easy visualization in rviz

### Dependencies

* ROS Noetic
* [`livox_ros_driver2`](https://github.com/Livox-SDK/livox_ros_driver2) (in same workspace)
* [Livox SDK 2.3.0](https://github.com/Livox-SDK/Livox-SDK) installed

### Usage

```bash
rosrun livox2pointcloud livox2pointcloud_node [livox_topic:="/livox/lidar"] [pointcloud_topic:="/livox/pointcloud2"]
```

### Result (Example from [Fast-LIVO](https://github.com/hku-mars/FAST-LIVO) dataset)

![livox](https://github.com/juliangaal/livox2pointcloud/assets/22290570/714c46c9-da35-46ad-a24b-6591695c6fef)
