# Small-satellites

本项目旨在为小卫星（Small Satellites）相关的软件开发、仿真和分析提供一套基础工具和示例。项目主要采用 C++ 编写，部分模块包含 HTML 以便于展示和交互。

## 项目简介

小卫星因其低成本、短开发周期和灵活性，近年来在科研、遥感、通信等领域得到广泛应用。本项目聚焦于小卫星任务的核心代码实现，包括但不限于：

- 姿态解算与控制
- 地面站通信模拟
- 数据处理与可视化

## 主要功能
- **姿态控制系统**  
  提供姿态传感器仿真、执行机构建模及姿态控制算法示例。

- **地面站链路仿真**  
  支持卫星与地面站之间的通信流程和数据链路建模。

- **数据可视化**  
  通过HTML前端，对关键仿真结果进行展示和交互。

## 目录结构

```
.
├── arduino_esp           # 嵌入式代码
├── backend               # 后端websocket，typescript
├── vitetest              # HTML前端,vue
└── README.md             # 项目说明
```

## 快速开始

1. **克隆项目**
   ```bash
   git clone https://github.com/ajuan223/Small-satellites.git
   cd Small-satellites
   ```

2. **编译项目**
   嵌入式使用 Arduino IDE (2.3.6)烧录，请按照报错提示安装开发板管理和库
   - mahony为姿态解算源码，无需烧录仅供参考。
   - arduino烧给arduino uno
   - camera烧给espcamera
   - esp01s烧给esp01s  
   后端yarn install初始化，yarn run run运行，前端已经集成在后端无需另外运行。后端启动后在浏览器输入localhost:3001即可访问。

## 贡献指南

欢迎各位对小卫星仿真、软件开发感兴趣的朋友提出建议或贡献代码。

1. Fork本仓库
2. 新建分支（`git checkout -b feature/my-feature`）
3. 提交更改（`git commit -am 'Add new feature'`）
4. 推送分支（`git push origin feature/my-feature`）
5. 发起 Pull Request

## 联系方式

如有问题或建议，请通过 Issues 或 Pull Requests 与我们联系。

---
**作者**: [ajuan223](https://github.com/ajuan223)
