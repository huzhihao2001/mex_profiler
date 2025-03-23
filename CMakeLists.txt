cmake_minimum_required(VERSION 3.10)
# ----------------------------------------
# 定义项目名称变量（后续只需修改此处）
# ----------------------------------------
set(MY_PROJECT_NAME "mex_profiler")  # 项目名称变量
project(${MY_PROJECT_NAME})             # 使用变量设置项目名

# ----------------------------------------
# 设置 MATLAB 依赖
# ----------------------------------------
set(MATLAB_ROOT "C:/Program Files/MATLAB/R2024a")  # 替换为你的 MATLAB 路径
include_directories("${MATLAB_ROOT}/extern/include")
include_directories("${MATLAB_ROOT}/extern/include/win64")
link_directories("${MATLAB_ROOT}/extern/lib/win64/microsoft")

# ----------------------------------------
# 启用 OpenMP
# ----------------------------------------
find_package(OpenMP REQUIRED)
if (OpenMP_FOUND)
    message(STATUS "OpenMP enabled")
    # 设置 OpenMP 编译选项
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(FATAL_ERROR "OpenMP not found")
endif()


# ----------------------------------------
# 启用 Visual Studio 性能探查器 (/PROFILE)
# ----------------------------------------
if (MSVC)
    # 1. 强制添加链接器标志 /PROFILE
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /PROFILE")
    
    # 2. 生成调试符号（PDB 文件）
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /DEBUG")
endif()

# ----------------------------------------
# 自动包含所有源文件
# ----------------------------------------
file(GLOB SOURCES "src/*.cpp" "src/*.c")

# ----------------------------------------
# 生成可执行文件
# ----------------------------------------
add_executable(${MY_PROJECT_NAME} ${SOURCES})

# ----------------------------------------
# 链接库
# ----------------------------------------
target_link_libraries(${MY_PROJECT_NAME}
    libmat.lib   # MATLAB 库
    libmx.lib
    libmex.lib
    libeng.lib
    OpenMP::OpenMP_CXX  # OpenMP
)

# ----------------------------------------
# 设置 C++ 标准
# ----------------------------------------
set_property(TARGET ${MY_PROJECT_NAME} PROPERTY CXX_STANDARD 11)