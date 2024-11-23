FROM ros:noetic-perception

ENV ROS_WS /opt/ros_ws

RUN apt update && apt install -y git 

# Install livox SDK
WORKDIR /tmp
RUN git clone https://github.com/Livox-SDK/Livox-SDK.git
WORKDIR /tmp/Livox-SDK/build
RUN cmake ..
RUN make -j$(nproc) && make install

# Build livox2pointcloud
RUN mkdir -p $ROS_WS/src
WORKDIR $ROS_WS/src
COPY . livox2pointcloud
RUN git clone https://github.com/Livox-SDK/livox_ros_driver2

WORKDIR $ROS_WS 
RUN bash -c '. /opt/ros/noetic/setup.bash && catkin_make -DCMAKE_BUILD_TYPE=Release --pkg livox_ros_driver2'
RUN bash -c '. /opt/ros/noetic/setup.bash && catkin_make -DCMAKE_BUILD_TYPE=Release'

COPY ./docker/entrypoint.sh /
ENTRYPOINT ["/entrypoint.sh"]
