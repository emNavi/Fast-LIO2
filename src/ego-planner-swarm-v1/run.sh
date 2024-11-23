#!/bin/bash
echo "[START] Ego_Planner_Swarm_V1 "

# 通过本脚本文件路径来获取 x152b 项目文件根目录
PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../.." && pwd )"

max_vel=1.0
max_acc=1.0
drone_id=1
# <!-- 1: use 2D Nav Goal to select goal  -->
# <!-- 2: use global waypoints below  -->
# <!-- 4: use REMOTE_TARGET  -->
# <!-- 5: use REMOTE_START  -->
flight_type=2
echo "Drone $drone_id  autonomous run in max_vel $max_vel max_acc:=$max_acc"

echo ${PROJECT_DIR}

if [ $? -eq 0 ] 
then
    source ${PROJECT_DIR}/devel/setup.bash;roslaunch ego_planner swarm_all_in_one.launch \
    drone_id:=$drone_id cx:=324.2211608886719 cy:=240.7614288330078 fx:=385.5018310546875 fy:=385.5018310546875 flight_type:=$flight_type max_vel:=$max_vel max_acc:=$max_acc && sleep 1;
else
    echo error
fi