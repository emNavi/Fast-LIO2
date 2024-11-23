#!/bin/bash
set -e

if [ ${ROS_DISTRO} == "foxy" ]; then
    source "${ROS_WS}/install/local_setup.bash"
else
    source "${ROS_WS}/devel/setup.bash"
fi

exec "$@"
