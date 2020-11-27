# Min CMake Ver. required
cmake_minimum_required(VERSION 2.8)

# Project Info
project(GLProjFrame)

# 我的解决方案
# 将所有源文件加入
#aux_source_directory(src DIR_SRC)


# 添加头文件和库文件查找目录
include_directories(include)
#message(${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

# 我的解决方案
# 指定生成目标
#add_executable(GLProjFrame ${DIR_SRC})

# 为添加头文件，解决方案来自https://www.cnblogs.com/dilex/p/11102152.html
# 查找指定目录下的所有.cpp与.h文件 并存放到指定变量名SC_FILES中
file(GLOB SC_FILES "src/*.cpp" "src/*.c" "src/*.h")

# 指定生成目标
add_executable(${PROJECT_NAME} ${SC_FILES})