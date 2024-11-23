# Fast-LIO + Ego-Planner

硬件设备：Mid360激光雷达

## 环境配置
```bash 
# Ceres 2.1.0
sudo apt-get -y install liblapack-dev libsuitesparse-dev libcxsparse3 libgflags-dev libgoogle-glog-dev libgtest-dev
wget -O ceres-solver.zip https://github.com/ceres-solver/ceres-solver/archive/refs/tags/2.1.0.zip
unzip -q ceres-solver.zip -d "${TRDPARTY_DIR}"
pushd "${TRDPARTY_DIR}/ceres-solver-2.1.0"
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=TRUE ..
make -j8
sudo make install

# Eigen 3.3.7
wget -O eigen3.zip <https://gitlab.com/libeigen/eigen/-/archive/3.3.7/eigen-3.3.7.zip>
unzip -q eigen3.zip -d "${TRDPARTY_DIR}"
pushd "${TRDPARTY_DIR}/eigen-3.3.7"
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=TRUE ..
make -j8
sudo make install
sudo ln -s /usr/include/eigen3/Eigen /usr/include/Eigen

# GTSAM
cd ~
git clone https://github.com/borglab/gtsam.git
mkdir build && cd build
cmake -D GTSAM_USE_SYSTEM_EIGEN=ON ..
make
sudo make install

# Livox SDK2
git clone https://github.com/Livox-SDK/Livox-SDK2.git
cd Livox-SDK2
cd build && cmake ..
make
sudo make install

# 编译项目
catkin_make

```

## 使用
插上Mid360激光雷达至电脑，并[配置好 livox_ros_driver2 驱动包中的 IP 地址](https://github.com/Livox-SDK/livox_ros_driver2?tab=readme-ov-file#4-lidar-config)
```bash 
# 开启 Fast_LIO 算法
bash ./run_fast_lio.sh 

# 开启 Ego-Planner 算法
bash ./run_ego_planner.sh
```