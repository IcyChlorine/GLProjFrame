cmake_minimum_required(VERSION 3.10)

project(GLProjFrame)

# set c++ standards
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_PATH src)
set(LIB_PATH lib)
set(INCLUDE_PATH include include/imgui)

file(GLOB GLPP_SRC ${SRC_PATH}/*.cpp ${SRC_PATH}/*.h)

file(GLOB THIRD_SRC ${SRC_PATH}/third/*.cpp ${SRC_PATH}/third/*.c) # we have imngui implementations and glad.c

message("GLPP_SRC is ${GLPP_SRC}")
message("CMAKE_SOURCE_DIR is ${CMAKE_SOURCE_DIR}")

add_executable(main ${GLPP_SRC} ${THIRD_SRC})
target_include_directories(main PRIVATE ${INCLUDE_PATH})
target_link_directories(main PRIVATE ${LIB_PATH})
target_link_libraries(main glfw3 gdi32 freetype assimp iconv zlibstatic)
add_custom_command(TARGET main POST_BUILD COMMAND 
    ${CMAKE_COMMAND} -E copy $<TARGET_FILE:main> ${CMAKE_SOURCE_DIR}/GLProjFrame/$<TARGET_FILE_NAME:main>)