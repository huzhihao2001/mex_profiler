
# MATLAB Mex函数性能测试框架 🚀

一个用于在visual studio中测试MATLAB Mex函数性能的轻量级框架。支持OpenMP加速与内置性能分析，解决MATLAB探查器无法直接分析Mex函数内部性能的问题。

![Build Status](https://img.shields.io/badge/build-passing-brightgreen) ![OpenMP](https://img.shields.io/badge/OpenMP-enabled-blue) ![MATLAB R2024a](https://img.shields.io/badge/MATLAB-R2024a-orange)[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/huzhihao2001/mex_profiler)

## 项目结构📂

```
mex_profiler/
├── data/               # 测试数据集（.mat文件）
├── src/                # 源码
│   ├── explore.c     	# Mex函数入口
│   └── main.cpp        # 性能测试主程序
├── CMakeLists.txt
├── LICENSE
└── README.md
```



## 安装步骤🔧

1. **克隆仓库**

   ```bash
   git clone https://github.com/huzhihao2001/mex_profiler.git
   cd mex_profiler
   ```

2. **配置环境变量（Windows）**

   添加以下路径至系统变量（改为自己MATLAB的相应路径）

   ```
   C:\Program Files\MATLAB\R2024a\extern\lib\win64
   C:\Program Files\MATLAB\R2024a\bin\win64
   ```

3. **修改CMake文件**

   替换`CMakeLists.txt`第5行的路径为本地MATLAB路径。

4. **CMake 编译**

   在项目根目录打开终端，输入以下命令

   ```
   mkdir build
   cd build
   cmake ..
   ```




## 使用说明🛠

1. **替换mex函数**

   用待检测的mex函数（例如`mymex.cpp`）替换掉`src\explore.c`文件

2. **生成测试数据**

   ```matlab
   %[A,B,C]=mymex(a,b,c,d)  假设mex函数名为mymex
   varargin={a,b,c,d};%按顺序输入的所有参数
   nrhs=4;%输入参数个数
   nlhs=3;%输出参数个数
   save test nlhs nrhs varargin
   ```

3. **替换测试数据**

   用生成的`test.mat`替换`data\test.mat`

4. **重构项目并设置测试入口**

   重新进行CMake编译后打开`build\mex_profiler.sln`,将`mex_profiler`设为启动项目后即可开始性能测试。



## 注意事项 ⚠️

- 目前尚未测试包含第三方库（如Eigen库）的情况，使用时需自行验证兼容性。
- 虽然项目标明了MATLAB R2024a，但较老的MATLAB版本可能也能使用，建议使用更新的MATLAB以确保兼容性。



## 许可证 📜

本项目采用 [MIT License](LICENSE)，允许自由使用、修改和分发。引用时请注明原作者。
