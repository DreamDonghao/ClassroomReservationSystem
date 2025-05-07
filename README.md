# 项目名称

教室预约系统

## 一、项目简介

[项目简介内容]

## 二、功能概述

[功能概述内容]

## 三、使用技术

- **构建工具**：CMake
- **前端**：HTML, CSS, JavaScript
- **后端**：C++, Crow轻量级web框架, vcpkg包管理工具
- **数据库**：MySQL

## 四、系统设计

- **Front类管理前端文件**：
  - 使用队列实现文件缓冲区，减少重复读取文件带来的性能开销。
  - 可根据实际情况自定义缓冲区大小。
  - 使用线程锁防止数据竞争：
    - 读取线程使用公共锁。
    - 写入线程使用私有锁。
  - 保证性能。
- **CORS中间件扩展Crow**：
  - 为所有路由自动加上跨域头。
- **使用关系型数据库，实现高效复杂操作**
  ```
    ┌────────────┐        ┌──────────────┐        ┌──────────────┐
    │   USERS    │◄───────┤ RESERVATIONS │───────►│ CLASSROOMS   │
    └────────────┘        └──────────────┘        └──────────────┘
          ▲                       ▲                      ▲
          │                       │                      │
          │                       │                classroom_id (PK)
          │                      year
          │                     month
          │                      day
          │                   time_period
        id (PK)               user_id (FK)
         name              classroom_id (FK)
  ```
## 五、系统部署运行与编译

### 环境依赖

- git
- CMake
- Ninja 或 make

### 1. 部署

- 解压压缩包。
- 或者克隆项目仓库（请确保已安装 **git**）：

```bash
# 克隆项目
git clone https://github.com/xxx/project-name.git
```

### 2. 运行

本项目提供以下预编译的可执行文件：

| 平台             | 可执行文件            |
| ---------------- | --------------------- |
| Windows (x86-64) | `a-Windows-64.exe`    |
| Windows (ARM)    | `a-Windows-arm64.exe` |
| Linux (x86-64)   | `a-linux-64.out`      |
| Linux (ARM)      | `a-linux-arm64.out`   |

若没有适合您机器的可执行文件，请按照下文编译说明自行编译。

### 3. 编译

请确保已安装CMake、Ninja（可选）、vcpkg等C++构建工具。在Windows下使用MSVC编译器，Linux下使用g++编译器，且工具需支持C++17标准。

#### 3.1 下载Crow与MySQL连接器

使用vcpkg安装Crow和MySQL C++连接器（您也可以通过其他方式添加这些库）。

**Windows**：

```bash
vcpkg install crow mysql-connector-cpp
```

**Linux**：

```bash
./vcpkg install crow mysql-connector-cpp
```

*注：默认情况下，vcpkg会根据您的环境选择包版本（Windows默认MSVC，Linux默认g++）。若下载的包版本不匹配，请手动指定版本，具体方法请查阅 [vcpkg官方文档](https://vcpkg.io/en/index.html)。*

#### 3.2 修改CMakeLists.txt中的vcpkg路径

在`CMakeLists.txt`文件中，将vcpkg路径修改为您环境中vcpkg的实际路径。

#### 3.3 构建并编译C++项目

1. 创建并进入build目录：

```bash
mkdir build
cd build
```

2. 根据您的构建工具选择以下方法之一：

**使用Ninja**：

```bash
cmake -G Ninja ..
ninja
```

**使用make**：

```bash
cmake ..
make
```

编译完成后，可执行文件将生成在build目录中。