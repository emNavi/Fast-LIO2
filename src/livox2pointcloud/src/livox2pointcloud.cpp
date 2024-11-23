#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <livox_ros_driver2/CustomMsg.h>
#include <livox_ros_driver2/CustomPoint.h>
#include <sensor_msgs/point_cloud2_iterator.h>


ros::Publisher pointcloud_pub;

sensor_msgs::PointCloud2 livox2pointcloud(const livox_ros_driver2::CustomMsg::ConstPtr& livox_msg) {
    sensor_msgs::PointCloud2 cloud_msg;
    cloud_msg.header.frame_id = livox_msg->header.frame_id;
    cloud_msg.header.stamp = livox_msg->header.stamp;
    cloud_msg.header.seq = livox_msg->header.seq;
    cloud_msg.height = 1;
    cloud_msg.width = livox_msg->points.size();
    cloud_msg.fields.resize(7);
    cloud_msg.fields[0].offset = 0;
    cloud_msg.fields[0].name = "offset_time";
    cloud_msg.fields[0].count = 1;
    cloud_msg.fields[0].datatype = sensor_msgs::PointField::UINT32;
    cloud_msg.fields[1].offset = 4;
    cloud_msg.fields[1].name = "x";
    cloud_msg.fields[1].count = 1;
    cloud_msg.fields[1].datatype = sensor_msgs::PointField::FLOAT32;
    cloud_msg.fields[2].offset = 8;
    cloud_msg.fields[2].name = "y";
    cloud_msg.fields[2].count = 1;
    cloud_msg.fields[2].datatype = sensor_msgs::PointField::FLOAT32;
    cloud_msg.fields[3].offset = 12;
    cloud_msg.fields[3].name = "z";
    cloud_msg.fields[3].count = 1;
    cloud_msg.fields[3].datatype = sensor_msgs::PointField::FLOAT32;
    cloud_msg.fields[4].offset = 16;
    cloud_msg.fields[4].name = "intensity";
    cloud_msg.fields[4].count = 1;
    cloud_msg.fields[4].datatype = sensor_msgs::PointField::FLOAT32;
    cloud_msg.fields[5].offset = 20;
    cloud_msg.fields[5].name = "tag";
    cloud_msg.fields[5].count = 1;
    cloud_msg.fields[5].datatype = sensor_msgs::PointField::UINT8;
    cloud_msg.fields[6].offset = 21;
    cloud_msg.fields[6].name = "line";
    cloud_msg.fields[6].count = 1;
    cloud_msg.fields[6].datatype = sensor_msgs::PointField::UINT8;
    cloud_msg.point_step = sizeof(livox_ros_driver2::CustomPoint);
    cloud_msg.row_step = cloud_msg.width * cloud_msg.point_step;
    cloud_msg.data.resize(cloud_msg.row_step);

    sensor_msgs::PointCloud2Iterator<uint32_t> iter_offset_time(cloud_msg, "offset_time");
    sensor_msgs::PointCloud2Iterator<float> iter_x(cloud_msg, "x");
    sensor_msgs::PointCloud2Iterator<float> iter_y(cloud_msg, "y");
    sensor_msgs::PointCloud2Iterator<float> iter_z(cloud_msg, "z");
    sensor_msgs::PointCloud2Iterator<float> iter_intensity(cloud_msg, "intensity");
    sensor_msgs::PointCloud2Iterator<uint8_t> iter_tag(cloud_msg, "tag");
    sensor_msgs::PointCloud2Iterator<uint8_t> iter_line(cloud_msg, "line");

    for (const auto & livox_p : livox_msg->points)
    {
        *iter_offset_time = livox_p.offset_time;
        *iter_x = livox_p.x;
        *iter_y = livox_p.y;
        *iter_z = livox_p.z;
        *iter_intensity = livox_p.reflectivity;
        *iter_tag = livox_p.tag;
        *iter_line = livox_p.line;

        ++iter_offset_time;
        ++iter_x;
        ++iter_y;
        ++iter_z;
        ++iter_intensity;
        ++iter_tag;
        ++iter_line;
    }

    return cloud_msg;
}

// Callback function for custom message subscriber
void customMsgCallback(const livox_ros_driver2::CustomMsg::ConstPtr& livox_msg)
{
    pointcloud_pub.publish(livox2pointcloud(livox_msg));
    ROS_INFO_STREAM("Converted livox_ros_driver2::CustomMsg to sensor_msgs::PointClouds2");
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "livox2pointcloud_node");
    ros::NodeHandle nh("~");  // Private node handle to access private parameters

    std::string pointcloud_topic;
    std::string livox_topic;
    nh.param<std::string>("pointcloud_topic", pointcloud_topic, "/livox/pointcloud2");
    nh.param<std::string>("livox_topic", livox_topic, "/livox/lidar");

    pointcloud_pub = nh.advertise<sensor_msgs::PointCloud2>(pointcloud_topic, 10);
    auto custom_msg_sub = nh.subscribe<livox_ros_driver2::CustomMsg>(livox_topic, 10, customMsgCallback);

    ros::spin();
    ros::spin();

    return 0;
}

