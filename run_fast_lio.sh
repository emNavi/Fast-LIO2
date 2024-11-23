#!/bin/bash
# set -x
# set -e

echo "[START] Fast-LIO2"

# 通过本脚本文件路径来获取项目文件根目录
PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


gnome-terminal -- bash -c "source ${PROJECT_DIR}/devel/setup.bash;roslaunch livox_ros_driver2 msg_MID360.launch"  && sleep 1;

gnome-terminal -- bash -c "source ${PROJECT_DIR}/devel/setup.bash;roslaunch fast_lio mapping_mid360.launch"  && sleep 1;

gnome-terminal -- bash -c "source ${PROJECT_DIR}/devel/setup.bash;roslaunch livox2pointcloud livox2pointcloud.launch"  && sleep 1;

gnome-terminal -- bash -c "source ${PROJECT_DIR}/devel/setup.bash;roslaunch foxglove_bridge foxglove_bridge.launch"  && sleep 1;

