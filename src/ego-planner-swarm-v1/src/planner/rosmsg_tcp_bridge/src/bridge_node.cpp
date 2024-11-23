#include <ros/ros.h>
#include <boost/thread.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <traj_utils/MultiBsplines.h>
#include <traj_utils/Bspline.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Empty.h>
#include <std_msgs/Float32MultiArray.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "bridge.hpp"
#include "pointcloud_msg.hpp"
#include "takeoff_msg.hpp"
#include "command_msg.hpp"
#include "land_msg.hpp"
#include "stop_msg.hpp"
#include "oneTraj_msg.hpp"
#include "odom_msg.hpp"
#include "command_msg.hpp"
using namespace std;

int udp_server_fd_, udp_send_fd_, udp_map_server_fd_, udp_map_send_fd_;
ros::Subscriber other_odoms_sub_, emergency_stop_sub_, one_traj_sub_, swarm_takeoff_sub_, swarm_land_sub_, swarm_command_sub_, swarm_pointcloud_sub_;
ros::Publisher other_odoms_pub_, emergency_stop_pub_, one_traj_pub_, swarm_takeoff_pub_, swarm_land_pub_, swarm_command_pub_, swarm_pointcloud_pub_;
string udp_ip_;
int drone_id_;
double odom_broadcast_freq_;
bool is_master;
bool pub_cloud;
struct sockaddr_in addr_udp_send_;
struct sockaddr_in addr_udp_map_send_;
traj_utils::MultiBsplinesPtr bsplines_msg_;
nav_msgs::OdometryPtr odom_msg_;
std_msgs::EmptyPtr stop_msg_;
traj_utils::BsplinePtr bspline_msg_;

std_msgs::Float32MultiArrayPtr takeoff_msg_;
std_msgs::Float32MultiArrayPtr land_msg_;
std_msgs::Float32MultiArrayPtr command_point_msg;
sensor_msgs::PointCloud2Ptr grid_map_msg;

int init_broadcast(const char *ip, const int port, sockaddr_in &addr_udp_send)
{
	int fd;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
	{
		ROS_ERROR("[bridge_node]Socket sender creation error!");
		exit(EXIT_FAILURE);
	}

	int so_broadcast = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast)) < 0)
	{
		cout << "Error in setting Broadcast option";
		exit(EXIT_FAILURE);
	}

	addr_udp_send.sin_family = AF_INET;
	addr_udp_send.sin_port = htons(port);

	if (inet_pton(AF_INET, ip, &addr_udp_send.sin_addr) <= 0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	return fd;
}

int udp_bind_to_port(const int port, int &server_fd)
{
	struct sockaddr_in address;
	int opt = 1;

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				   &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	// Forcefully attaching socket to the port
	if (bind(server_fd, (struct sockaddr *)&address,
			 sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	return server_fd;
}

void odom_sub_udp_cb(const nav_msgs::OdometryPtr &msg)
{

	static ros::Time t_last;
	ros::Time t_now = ros::Time::now();
	if ((t_now - t_last).toSec() * odom_broadcast_freq_ < 1.0)
	{
		return;
	}
	t_last = t_now;

	msg->child_frame_id = string("drone_") + std::to_string(drone_id_);

	int len = serializeOdom(msg);

	if (sendto(udp_send_fd_, udp_send_buf_, len, 0, (struct sockaddr *)&addr_udp_send_, sizeof(addr_udp_send_)) <= 0)
	{
		ROS_ERROR("UDP SEND ERROR (1)!!!");
	}
}

void takeoff_command_cb(const std_msgs::Float32MultiArray &msg)
{
	int len = serializeTakeoff(msg);

	if (sendto(udp_send_fd_, udp_send_buf_, len, 0, (struct sockaddr *)&addr_udp_send_, sizeof(addr_udp_send_)) <= 0)
	{
		ROS_ERROR("UDP SEND ERROR (2)!!!");
	}
}

void land_command_cb(const std_msgs::Float32MultiArray &msg)
{
	int len = serializeLand(msg);

	if (sendto(udp_send_fd_, udp_send_buf_, len, 0, (struct sockaddr *)&addr_udp_send_, sizeof(addr_udp_send_)) <= 0)
	{
		ROS_ERROR("UDP SEND ERROR (2)!!!");
	}
}

void next_command_cb(const std_msgs::Float32MultiArray &msg)
{
	int len = serializeNextcommand(msg);

	if (sendto(udp_send_fd_, udp_send_buf_, len, 0, (struct sockaddr *)&addr_udp_send_, sizeof(addr_udp_send_)) <= 0)
	{
		ROS_ERROR("UDP SEND ERROR (2)!!!");
	}
}

void emergency_stop_sub_udp_cb(const std_msgs::EmptyPtr &msg)
{

	int len = serializeStop(msg);

	if (sendto(udp_send_fd_, udp_send_buf_, len, 0, (struct sockaddr *)&addr_udp_send_, sizeof(addr_udp_send_)) <= 0)
	{
		ROS_ERROR("UDP SEND ERROR (2)!!!");
	}
}

void one_traj_sub_udp_cb(const traj_utils::BsplinePtr &msg)
{

	int len = serializeOneTraj(msg);

	if (sendto(udp_send_fd_, udp_send_buf_, len, 0, (struct sockaddr *)&addr_udp_send_, sizeof(addr_udp_send_)) <= 0)
	{
		ROS_ERROR("UDP SEND ERROR (3)!!!");
	}
}
uint32_t send_pkg_id=0;
void drone_gridmap_cb(const sensor_msgs::PointCloud2Ptr &msg)
{

	static ros::Time t_last;
	ros::Time t_now = ros::Time::now();
	// max send freq 3hz
	if ((t_now - t_last).toSec() * 3.0 < 1.0)
	{
		return;
	}
	t_last = t_now;

	int len = serializePointcloud(msg, send_map_buf_full_, drone_id_);
	if (len < BUF_LEN)
	{
		if (sendto(udp_map_send_fd_, send_map_buf_full_, len, 0, (struct sockaddr *)&addr_udp_map_send_, sizeof(addr_udp_map_send_)) <= 0)
		{
			ROS_ERROR("UDP SEND ERROR (3)!!!");
		}
	}
	else
	{
		uint16_t drone_id;
		send_pkg_id++;
		for (int i = 0; i <= len / BUF_LEN; i++)
		{
			int header_len = createSegmentHeader(send_map_buf_, i, len / BUF_LEN, send_pkg_id, drone_id);
			memcpy(send_map_buf_ + header_len, send_map_buf_full_ + i * BUF_LEN, BUF_LEN);
			int send_len;
			if (i == len / BUF_LEN)
			{
				send_len = header_len + len - (i * BUF_LEN);
			}
			else
			{
				send_len = header_len + BUF_LEN;
			}

			if (sendto(udp_map_send_fd_, send_map_buf_, send_len, 0, (struct sockaddr *)&addr_udp_map_send_, sizeof(addr_udp_map_send_)) <= 0)
			{
				ROS_ERROR("UDP SEND ERROR (3)!!!");
			}
		}
	}
}

int32_t current_pkg_id = 0;
uint16_t current_drone_id = 1000;
int32_t current_recv_piece_num = 0;
int32_t current_recv_length = 0;

void udp_map_recv()
{
	int valread;
	// int pointCloudState=;
	struct sockaddr_in addr_client;
	socklen_t addr_len;
	// Connect for recv
	if (udp_bind_to_port(UDP_MAP_PORT, udp_map_server_fd_) < 0)
	{
		ROS_ERROR("[bridge_node]Socket recever creation error!");
		exit(EXIT_FAILURE);
	}
	while (true)
	{
		if ((valread = recvfrom(udp_map_server_fd_, recv_map_buf_, BUF_LEN+BUF_LEN_SHORT, 0, (struct sockaddr *)&addr_client, (socklen_t *)&addr_len)) < 0)
		{
			perror("recvfrom error:");
			exit(EXIT_FAILURE);
		}
		char *ptr = recv_map_buf_;
		switch (*((MESSAGE_TYPE *)ptr))
		{
		case MESSAGE_TYPE::POINTCLOUD:
		{
			if (valread == deserializePointcloud(grid_map_msg, recv_map_buf_))
			{
				swarm_pointcloud_pub_.publish(*grid_map_msg);
			}
			else
			{
				ROS_ERROR("Received message length not matches the grid_map!!!");
				continue;
			}
			break;
		}
		case MESSAGE_TYPE::POINTCLOUD_SEGMENT:
		{
			int segment_index, segment_number;
			uint32_t pkg_id;
			uint16_t drone_id;
			// just one frame
			int header_len = parseSegmentHeader(recv_map_buf_, &segment_index, &segment_number, &pkg_id, &drone_id);

			if (drone_id != current_drone_id)
			{
				ROS_ERROR_STREAM("msg drop out !!!,drone_id!=current_drone_id");
				current_drone_id = drone_id;
				current_recv_piece_num = 0;
				current_pkg_id = pkg_id;
			}
			if (pkg_id - current_pkg_id > 0)
			{
				current_pkg_id = pkg_id;
				current_recv_piece_num = 0;
			}

			if (pkg_id - current_pkg_id == 0)
			{
				memcpy(recv_map_buf_full_ + segment_index * BUF_LEN, recv_map_buf_ + header_len, valread - header_len);
				current_recv_length += (valread - header_len);
				if (current_recv_piece_num == segment_number)
				{
					if ((current_recv_length) == deserializePointcloud(grid_map_msg, recv_map_buf_full_))
					{
						swarm_pointcloud_pub_.publish(*grid_map_msg);
					}
					else
					{
						ROS_ERROR_STREAM("Stream length not eq" << current_recv_length);
					}
					current_recv_length = 0;
					current_recv_piece_num = 0;
				}
				current_recv_piece_num++;

			}
			else
			{
				ROS_ERROR_STREAM("msg drop out !!!,pkg_id " << pkg_id << "<current_pkg_id " << current_pkg_id);
			}

			break;
		}
		default:
			ROS_ERROR_STREAM("Unknown received message???"
							 << "TYPE" << *((MESSAGE_TYPE *)ptr));
			break;
		}
	}
}
void udp_recv_fun()
{
	int valread;
	struct sockaddr_in addr_client;
	socklen_t addr_len;

	// Connect for recv
	if (udp_bind_to_port(UDP_PORT, udp_server_fd_) < 0)
	{
		ROS_ERROR("[bridge_node]Socket recever creation error!");
		exit(EXIT_FAILURE);
	}

	while (true)
	{
		if ((valread = recvfrom(udp_server_fd_, udp_recv_buf_, BUF_LEN, 0, (struct sockaddr *)&addr_client, (socklen_t *)&addr_len)) < 0)
		{
			perror("recvfrom error:");
			exit(EXIT_FAILURE);
		}

		char *ptr = udp_recv_buf_;
		switch (*((MESSAGE_TYPE *)ptr))
		{
		case MESSAGE_TYPE::STOP:
		{

			if (valread == sizeof(std_msgs::Empty))
			{
				if (valread == deserializeStop(stop_msg_))
				{
					emergency_stop_pub_.publish(*stop_msg_);
				}
				else
				{
					ROS_ERROR("Received message length not matches the sent one (1)!!!");
					continue;
				}
			}

			break;
		}

		case MESSAGE_TYPE::ODOM:
		{
			if (valread == deserializeOdom(odom_msg_))
			{
				other_odoms_pub_.publish(*odom_msg_);
			}
			else
			{
				ROS_ERROR("Received message length not matches the sent one (2)!!!");
				continue;
			}

			break;
		}

		case MESSAGE_TYPE::ONE_TRAJ:
		{

			if (valread == deserializeOneTraj(bspline_msg_))
			{
				one_traj_pub_.publish(*bspline_msg_);
			}
			else
			{
				ROS_ERROR("Received message length not matches the sent one (3)!!!");
				continue;
			}

			break;
		}
		case MESSAGE_TYPE::TAKEOFF:
		{
			ROS_INFO("RECV TAKEOFF");
			if (is_master)
			{
				continue;
			}
			if (valread == deserializeTakeoff(takeoff_msg_))
			{
				swarm_takeoff_pub_.publish(*takeoff_msg_);
			}
			else
			{
				ROS_ERROR("Received message length not matches the Takeoff (1)!!!");
				continue;
			}
			break;
		}
		case MESSAGE_TYPE::LAND:
		{
			ROS_INFO("RECV LAND");
			if (is_master)
			{
				continue;
			}
			if (valread == deserializeTakeoff(land_msg_))
			{
				swarm_land_pub_.publish(*land_msg_);
			}
			else
			{
				ROS_ERROR("Received message length not matches the Land (1)!!!");
				continue;
			}

			break;
		}
		case MESSAGE_TYPE::ONE_POINT:
		{
			ROS_INFO("RECV ONE_POINT");
			if (is_master)
			{
				continue;
			}
			if (valread == deserializeNextCommand(land_msg_))
			{
				swarm_command_pub_.publish(*land_msg_);
			}
			else
			{
				ROS_ERROR("Received message length not matches the Land (1)!!!");
				continue;
			}

			break;
		}

		default:
			ROS_ERROR("Unknown received message???");
			break;
		}
	}
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "rosmsg_tcp_bridge");
	ros::NodeHandle nh("~");

	nh.param("broadcast_ip", udp_ip_, string("127.0.0.255"));
	nh.param("drone_id", drone_id_, -1);
	nh.param("odom_max_freq", odom_broadcast_freq_, 1000.0);
	nh.param("is_master", is_master, false);
	nh.param("pub_cloud",pub_cloud,false);

	bsplines_msg_.reset(new traj_utils::MultiBsplines);
	odom_msg_.reset(new nav_msgs::Odometry);
	stop_msg_.reset(new std_msgs::Empty);
	bspline_msg_.reset(new traj_utils::Bspline);

	takeoff_msg_.reset(new std_msgs::Float32MultiArray);
	land_msg_.reset(new std_msgs::Float32MultiArray);
	command_point_msg.reset(new std_msgs::Float32MultiArray);
	grid_map_msg.reset(new sensor_msgs::PointCloud2);

	if (drone_id_ == -1)
	{
		ROS_ERROR("Wrong drone_id!");
		exit(EXIT_FAILURE);
	}

	other_odoms_sub_ = nh.subscribe("my_odom", 10, odom_sub_udp_cb, ros::TransportHints().tcpNoDelay());
	other_odoms_pub_ = nh.advertise<nav_msgs::Odometry>("/others_odom", 10);

	// emergency_stop_sub_ = nh.subscribe("emergency_stop_broadcast", 10, emergency_stop_sub_udp_cb, ros::TransportHints().tcpNoDelay());
	// emergency_stop_pub_ = nh.advertise<std_msgs::Empty>("emergency_stop_recv", 10);

	one_traj_sub_ = nh.subscribe("/broadcast_bspline", 100, one_traj_sub_udp_cb, ros::TransportHints().tcpNoDelay());
	one_traj_pub_ = nh.advertise<traj_utils::Bspline>("/broadcast_bspline2", 100);

	swarm_takeoff_pub_ = nh.advertise<std_msgs::Float32MultiArray>("/swarm_takeoff", 100);
	swarm_land_pub_ = nh.advertise<std_msgs::Float32MultiArray>("/swarm_land", 100);
	swarm_command_pub_ = nh.advertise<std_msgs::Float32MultiArray>("/swarm_command", 100);
	if(pub_cloud)
		swarm_pointcloud_pub_ = nh.advertise<sensor_msgs::PointCloud2>("/swarm_pointcloud", 100);

	if (is_master)
	{
		swarm_takeoff_sub_ = nh.subscribe("/swarm_takeoff", 100, takeoff_command_cb, ros::TransportHints().tcpNoDelay());
		swarm_land_sub_ = nh.subscribe("/swarm_land", 100, land_command_cb, ros::TransportHints().tcpNoDelay());
		swarm_command_sub_ = nh.subscribe("/swarm_command", 100, next_command_cb, ros::TransportHints().tcpNoDelay());
	}
	else
	{
		if(pub_cloud)
			swarm_pointcloud_sub_ = nh.subscribe("gridmap", 100, drone_gridmap_cb, ros::TransportHints().tcpNoDelay());
	}

	boost::thread udp_recv_thd(udp_recv_fun);
	udp_recv_thd.detach();
	ros::Duration(0.1).sleep();

	if (is_master)
	{
		boost::thread udp_map_recv_thd(udp_map_recv);
		udp_map_recv_thd.detach();
		ros::Duration(0.1).sleep();
	}

	// UDP connect
	udp_send_fd_ = init_broadcast(udp_ip_.c_str(), UDP_PORT, addr_udp_send_);
	udp_map_send_fd_ = init_broadcast(udp_ip_.c_str(), UDP_MAP_PORT, addr_udp_map_send_);
	cout << "[rosmsg_tcp_bridge] start running" << endl;

	ros::spin();

	close(udp_server_fd_);
	close(udp_send_fd_);
	close(udp_map_server_fd_);
	close(udp_map_send_fd_);
	return 0;
}
