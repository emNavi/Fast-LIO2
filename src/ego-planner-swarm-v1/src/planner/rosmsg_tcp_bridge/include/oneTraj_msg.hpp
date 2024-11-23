#ifndef __ONE_TRAJ_MSG_HPP
#define __ONE_TRAJ_MSG_HPP
#include "bridge.hpp"
#include <traj_utils/Bspline.h>
#include <ros/ros.h>

int deserializeOneTraj(traj_utils::BsplinePtr &msg)
{
  char *ptr = udp_recv_buf_;

  ptr += sizeof(MESSAGE_TYPE);

  msg->drone_id = *((int32_t *)ptr);
  ptr += sizeof(int32_t);
  msg->order = *((int32_t *)ptr);
  ptr += sizeof(int32_t);
  msg->start_time.fromSec(*((double *)ptr));
  ptr += sizeof(double);
  msg->traj_id = *((int64_t *)ptr);
  ptr += sizeof(int64_t);
  msg->yaw_dt = *((double *)ptr);
  ptr += sizeof(double);
  msg->knots.resize(*((size_t *)ptr));
  ptr += sizeof(size_t);
  for (size_t j = 0; j < msg->knots.size(); j++)
  {
    msg->knots[j] = *((double *)ptr);
    ptr += sizeof(double);
  }

  msg->pos_pts.resize(*((size_t *)ptr));
  ptr += sizeof(size_t);
  for (size_t j = 0; j < msg->pos_pts.size(); j++)
  {
    msg->pos_pts[j].x = *((double *)ptr);
    ptr += sizeof(double);
    msg->pos_pts[j].y = *((double *)ptr);
    ptr += sizeof(double);
    msg->pos_pts[j].z = *((double *)ptr);
    ptr += sizeof(double);
  }

  msg->yaw_pts.resize(*((size_t *)ptr));
  ptr += sizeof(size_t);
  for (size_t j = 0; j < msg->yaw_pts.size(); j++)
  {
    msg->yaw_pts[j] = *((double *)ptr);
    ptr += sizeof(double);
  }

  return ptr - udp_recv_buf_;
}

int serializeOneTraj(const traj_utils::BsplinePtr &msg)
{
  char *ptr = udp_send_buf_;

  unsigned long total_len = 0;
  total_len += sizeof(int32_t) + sizeof(int32_t) + sizeof(double) + sizeof(int64_t) + sizeof(double);
  total_len += sizeof(size_t) + msg->knots.size() * sizeof(double);
  total_len += sizeof(size_t) + (3 * msg->pos_pts.size()) * sizeof(double);
  total_len += sizeof(size_t) + msg->yaw_pts.size() * sizeof(double);
  if (total_len + 1 > BUF_LEN)
  {
    ROS_ERROR("[bridge_node] Topic is too large, please enlarge BUF_LEN (2)");
    return -1;
  }

  *((MESSAGE_TYPE *)ptr) = MESSAGE_TYPE::ONE_TRAJ;
  ptr += sizeof(MESSAGE_TYPE);

  *((int32_t *)ptr) = msg->drone_id;
  ptr += sizeof(int32_t);
  *((int32_t *)ptr) = msg->order;
  ptr += sizeof(int32_t);
  *((double *)ptr) = msg->start_time.toSec();
  ptr += sizeof(double);
  *((int64_t *)ptr) = msg->traj_id;
  ptr += sizeof(int64_t);
  *((double *)ptr) = msg->yaw_dt;
  ptr += sizeof(double);

  *((size_t *)ptr) = msg->knots.size();
  ptr += sizeof(size_t);
  for (size_t j = 0; j < msg->knots.size(); j++)
  {
    *((double *)ptr) = msg->knots[j];
    ptr += sizeof(double);
  }

  *((size_t *)ptr) = msg->pos_pts.size();
  ptr += sizeof(size_t);
  for (size_t j = 0; j < msg->pos_pts.size(); j++)
  {
    *((double *)ptr) = msg->pos_pts[j].x;
    ptr += sizeof(double);
    *((double *)ptr) = msg->pos_pts[j].y;
    ptr += sizeof(double);
    *((double *)ptr) = msg->pos_pts[j].z;
    ptr += sizeof(double);
  }

  *((size_t *)ptr) = msg->yaw_pts.size();
  ptr += sizeof(size_t);
  for (size_t j = 0; j < msg->yaw_pts.size(); j++)
  {
    *((double *)ptr) = msg->yaw_pts[j];
    ptr += sizeof(double);
  }

  return ptr - udp_send_buf_;
}

#endif