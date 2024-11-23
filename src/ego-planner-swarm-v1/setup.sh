#!/usr/bin/env bash
# set -x
# set -e

# 获取项目根路径
PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../.." && pwd )"
# Ego_planner_swarm_v1
EGO_PLANNER_SWARM_V1_DIR="${PROJECT_DIR}/src/ego_planner_swarm_v1"

if [ ! -d "${EGO_PLANNER_SWARM_V1_DIR}" ]; then
    echo "Downloading Ego_planner_swarm_v1..."
    git clone https://gitee.com/Derkai52/ego_planner_swarm_v1.git "${EGO_PLANNER_SWARM_V1_DIR}"
    # git clone https://github.com/emNavi/ego_planner_swarm_v1.git "${EGO_PLANNER_SWARM_V1_DIR}"
else
    echo "Ego_planner_swarm_v1 is already installed."
fi

catkin_make

popd >/dev/null

set +x
echo ""
echo "************************************"
echo "Ego_planner_swarm_v1 setup completed successfully!"
echo "************************************"