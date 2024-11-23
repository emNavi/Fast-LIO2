#ifndef __LAND_MSG_HPP
#define __LAND_MSG_HPP
#include "bridge.hpp"
#include <std_msgs/Float32MultiArray.h>

int deserializeLand(std_msgs::Float32MultiArrayPtr &msg)
{
  char *ptr = udp_recv_buf_;
  ptr += sizeof(MESSAGE_TYPE);
  float len = *((float *)ptr);
  msg->data.resize(len+1);
  msg->data[0] = len;
  ptr += sizeof(float);

  for(int i=0;i<len;i++)
  {
    msg->data[i+1] = *((float *)ptr);
    ptr += sizeof(float);
  }


  return ptr - udp_recv_buf_;
}
int serializeLand(const std_msgs::Float32MultiArray &msg)
{
  char *ptr = udp_send_buf_;

  *((MESSAGE_TYPE *)ptr) = MESSAGE_TYPE::LAND;
  ptr += sizeof(MESSAGE_TYPE);

  float len = msg.data[0];
  *((float *)ptr) = len;
  ptr += sizeof(float);
  for(int i=0;i<len;i++)
  {
    *((float *)ptr) = msg.data[i + 1];
    ptr += sizeof(float);
  }

  return ptr - udp_send_buf_;
}

#endif