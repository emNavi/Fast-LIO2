#ifndef __STOP_MSG_HPP
#define __STOP_MSG_HPP
#include "bridge.hpp"
#include <std_msgs/Empty.h>

int deserializeStop(std_msgs::EmptyPtr &msg)
{
  char *ptr = udp_recv_buf_;

  return ptr - udp_recv_buf_;
}
int serializeStop(const std_msgs::EmptyPtr &msg)
{
  char *ptr = udp_send_buf_;

  *((MESSAGE_TYPE *)ptr) = MESSAGE_TYPE::STOP;
  ptr += sizeof(MESSAGE_TYPE);

  return ptr - udp_send_buf_;
}
#endif