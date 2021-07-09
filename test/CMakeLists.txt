# Min CMake Ver. required
cmake_minimum_required(VERSION 3.0)

# Project Info
project(test)

# 将所有源文件加入
aux_source_directory(src DIR_SRC)

# 指定生成目标
add_executable(test ${DIR_SRC})