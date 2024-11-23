# 【2021ICRA】Ego-Planner-Swarm-V1 规划算法

Ego-Planner-Swarm-V1 是一个在复杂环境下去中心化异步系统性的多无人机自动导航解决方案，是 Ego-Planner 在群体的拓展。本算法使用轻量级的拓扑学路径生成方法、可依赖的路径分享网络，使得多无人机能够生成安全、平坦、满足动态可行性的路径。

- 论文链接：https://arxiv.org/pdf/2011.04183.pdf
- 源项目地址：https://github.com/ZJU-FAST-Lab/ego-planner-swarm.git

## 我们的更改：
- （默认未启用）增加了 MID360 的功能，在开启 `USE_MID360_CLOUD` 选项时，使用点云输入：
  ```cmake
  # 可以手动在 ego_planner_swarm_v1/src/planner/plan_env/CMakeLists.txt 下取消注释
  add_compile_definitions(USE_MID360_CLOUD)
  ```