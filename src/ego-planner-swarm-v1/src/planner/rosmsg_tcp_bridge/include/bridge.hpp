#ifndef __BRIDGE_HPP_
#define __BRIDGE_HPP_
// 8080 8060 already in used by gitlab, TCP is useless,give a random port value
#define PORT 8099
#define UDP_MAP_PORT 58099 
#define UDP_PORT 8081

// MAX 65535-20(header)-8(udp_header)
#define BUF_LEN 30000    // about 60KB
#define BUF_FULL_LEN 3000000    // about 3MB
#define BUF_LEN_SHORT 1024 // 1KB
enum MESSAGE_TYPE
{
  ODOM = 888,
  MULTI_TRAJ,
  ONE_TRAJ,
  STOP,
  TAKEOFF,
  LAND,
  ONE_POINT,
  POINTCLOUD,
  POINTCLOUD_SEGMENT

} massage_type_;
char send_map_buf_full_[BUF_FULL_LEN], recv_map_buf_full_[BUF_FULL_LEN],send_map_buf_[BUF_LEN+BUF_LEN_SHORT],recv_map_buf_[BUF_LEN+BUF_LEN_SHORT], udp_recv_buf_[BUF_LEN], udp_send_buf_[BUF_LEN];
char udp_over_max_len_save[BUF_LEN];
#endif