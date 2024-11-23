#ifndef __COMMAND_MSG_HPP
#define __COMMAND_MSG_HPP
#include "bridge.hpp"

#include <std_msgs/Float32MultiArray.h>

int deserializeNextCommand(std_msgs::Float32MultiArrayPtr &msg)
{
  char *ptr = udp_recv_buf_;
  ptr += sizeof(MESSAGE_TYPE);
  float drone_id = *((float *)ptr);
  msg->data.resize(4);
  msg->data[0] = drone_id;
  ptr += sizeof(float);

  for(int i=0;i<3;i++)
  {
    msg->data[i+1] = *((float *)ptr);
    ptr += sizeof(float);
  }

  return ptr - udp_recv_buf_;
}
int serializeNextcommand(const std_msgs::Float32MultiArray &msg)
{
  char *ptr = udp_send_buf_;

  *((MESSAGE_TYPE *)ptr) = MESSAGE_TYPE::ONE_POINT;
  ptr += sizeof(MESSAGE_TYPE);

  float drone_id = msg.data[0];
  *((float *)ptr) = drone_id;
  ptr += sizeof(float);
  for(int i=0;i<3;i++)
  {
    *((float *)ptr) = msg.data[i + 1];
    ptr += sizeof(float);
  }

  return ptr - udp_send_buf_;
}

#endif