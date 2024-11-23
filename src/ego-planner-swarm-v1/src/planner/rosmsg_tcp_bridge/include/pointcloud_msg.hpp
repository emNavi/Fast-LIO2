// just support gridmap
// just support gridmap
// just support gridmap xyz
// A single transmission of 40000 points with xyz type
// MAX +- 327.67
// resolution 0.01m
#ifndef __POINTCLOUD_MSG_HPP
#define __POINTCLOUD_MSG_HPP
#include "sensor_msgs/PointCloud2.h"
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include "bridge.hpp"

int deserializePointcloud(sensor_msgs::PointCloud2Ptr &msg,char* recv_buf)
{
  // MESSAGE_TYPE,Drone_id,timestamp,size
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);

  char *ptr = recv_buf;

  ptr += sizeof(MESSAGE_TYPE);
  int32_t drone_id_temp = *((int32_t *)ptr);
  ptr += sizeof(int32_t);
  double time_stamp = *((double *)ptr);
  ptr += sizeof(double);
  // ROS_INFO_STREAM(time_stamp);

  size_t points_size = *((size_t *)ptr);
  ptr += sizeof(size_t);
  // ROS_INFO_STREAM(points_size);

  for (size_t i = 0; i <  points_size; i++)
  {
    pcl::PointXYZ point;
    point.x = (*((int16_t *)ptr))/100.0;
    ptr += sizeof(int16_t);
    point.y = (*((int16_t *)ptr))/100.0;
    ptr += sizeof(int16_t);
    point.z = (*((int16_t *)ptr))/100.0;
    ptr += sizeof(int16_t);
    cloud->push_back(point);
  }

  unsigned long total_len = 0;
  total_len += sizeof(MESSAGE_TYPE) + sizeof(int32_t) +sizeof(double) + sizeof(size_t);
  total_len += cloud->size()*sizeof(int16_t)*3;

  // ROS_INFO_STREAM("total len recv "<< total_len);


  pcl::toROSMsg(*cloud,*msg);
  msg->header.stamp.fromSec(time_stamp);
  msg->header.frame_id=std::string("drone_") + std::to_string(drone_id_temp);


  return ptr - recv_buf;
}
int serializePointcloud(const sensor_msgs::PointCloud2Ptr &msg,char* send_buf,int32_t drone_id)
{
  // MESSAGE_TYPE,Drone_id,timestamp,size
  char *ptr = send_buf;
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
  pcl::fromROSMsg(*msg,*cloud);
  unsigned long total_len = 0;
  total_len += sizeof(MESSAGE_TYPE) + sizeof(int32_t) +sizeof(double) + sizeof(size_t);
  total_len += cloud->size()*sizeof(int16_t)*3;

  if (total_len + 1 > BUF_FULL_LEN)
  {
    ROS_ERROR("[bridge_node] Topic is too large, please enlarge BUF_FULL_LEN");
    return -1;
  }

  *((MESSAGE_TYPE *)ptr) = MESSAGE_TYPE::POINTCLOUD;
  ptr += sizeof(MESSAGE_TYPE);

  *((int32_t *)ptr) = drone_id;//this is drone id 
  ptr += sizeof(int32_t);

  *((double *)ptr) = msg->header.stamp.toSec();//this is drone id 
  ptr += sizeof(double);
  *((size_t *)ptr) = cloud->size();

  ptr += sizeof(size_t);
  for (size_t i = 0; i < cloud->size(); i++)
  {
    *((int16_t *)ptr) = static_cast<int16_t>(cloud->points[i].x*100);
    ptr += sizeof(int16_t);
    *((int16_t *)ptr) = static_cast<int16_t>(cloud->points[i].y*100);
    ptr += sizeof(int16_t);   
    *((int16_t *)ptr) = static_cast<int16_t>(cloud->points[i].z*100);
    ptr += sizeof(int16_t);
  }
  return ptr - send_buf;
}
// HEADER
// - type(MESSAGE_TYPE) MESSAGE_TYPE
// - type(uint32_t) pkg_id
// - type(uint16_t) drone_id
// - type(uint16_t) segment_num
// - type(uint16_t) segment_index
// POINTCLOUD_MSG

uint16_t getSegmentHeaderLen(){
  unsigned long total_len = 0;
  total_len += sizeof(MESSAGE_TYPE);
  total_len += sizeof(uint32_t);
  total_len += sizeof(uint16_t);
  total_len += sizeof(uint16_t);
  total_len += sizeof(uint16_t);
  return total_len;
}

int createSegmentHeader(char* send_buf,int segment_index,int segment_num,uint32_t pkg_id,uint16_t drone_id){
  char *ptr = send_buf;

  *((MESSAGE_TYPE *)ptr) = MESSAGE_TYPE::POINTCLOUD_SEGMENT;
  ptr += sizeof(MESSAGE_TYPE);

  *((uint32_t *)ptr) = pkg_id;
  ptr += sizeof(uint32_t);

  *((uint16_t *)ptr) = drone_id;
  ptr += sizeof(uint16_t);

  *((uint16_t *)ptr) = segment_num;
  ptr += sizeof(uint16_t);

  *((uint16_t *)ptr) = segment_index;//this is segment index
  ptr += sizeof(uint16_t);
  return getSegmentHeaderLen();
}

uint16_t parseSegmentHeader(char* recv_buf,int *segment_index,int *segment_num,uint32_t *pkg_id,uint16_t *drone_id){
  char *ptr = recv_buf;
  ptr += sizeof(MESSAGE_TYPE);

  *pkg_id = *((uint32_t *)ptr);
  ptr += sizeof(uint32_t);

  *drone_id = *((uint16_t *)ptr);
  ptr += sizeof(uint16_t);

  *segment_num = *((uint16_t *)ptr);
  ptr += sizeof(uint16_t);

  *segment_index = *((uint16_t *)ptr);
  ptr += sizeof(uint16_t);
  
  return getSegmentHeaderLen();
}
#endif