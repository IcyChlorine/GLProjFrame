CXX = g++
CXXFLAGS = -std=c++11
DEBUG = false

SRC_PATH = ./src
LIB_PATH = ./lib
INCLUDE_PATH = ./include

HEADERS = $(wildcard $(SRC_PATH)/*.h)
SRCS = $(wildcard $(SRC_PATH)/*.cpp $(SRC_PATH)/*.c)
# 链接glfw3时必须同时链接gdi32
# 另外，注意依赖顺序。
LIBS = -lglfw3 -lgdi32 -lfreetype -lassimp -liconv -lzlibstatic

TARGET_FILENAME = main.exe

# some little matic
ifeq ($(DEBUG), true)
	CXXFLAGS += -Og -g
else
	CXXFLAGS += -O2
endif

# for incremental compilation #
.cpp.o:
	$(CXX) $(CXXFLAGS) -I $(INCLUDE_PATH) -c $<  -o $@

.c.o:
	$(CXX) $(CXXFLAGS) -I $(INCLUDE_PATH) -c $<  -o $@

OBJ_FILES = $(SRCS:.cpp=.o)

main.exe: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(TARGET_FILENAME) -I $(INCLUDE_PATH) -L $(LIB_PATH) $(LIBS)

# macroscopic tasks #
.PHONY: main all clean run count

main: main.exe
# pass

all: main
# pass

clean:
	rm -f $(TARGET_FILENAME)
	rm -f src/*.o

run: all
	./$(TARGET_FILENAME)

SHADER_PATH = $(SRC_PATH)/shaders
SHADERS = $(wildcard $(SHADER_PATH)/*.*)
AUXS = makefile
count:
	cloc $(HEADERS) $(SRCS) $(SHADERS) $(AUXS)

# resolving third-party library dependencies automatically

# For most time, installing a third-party library from online can be decomposed into three steps:
# 1. downloading. Download the source code of the library from online url, 
#                 like "git clone ..."
# 2. build. Build the binaries from source code, if necessary,
#           like "cmake -B build ... && cd build && make"
# 3. install. Copy the binaries just built and headers required into proper directories,
#           like "cp .../build/code/libxxx.a lib/libxxx.a"

.PHONY: dep assimp
dep: dep_dir assimp
	@echo "Building dependencies..."

.PHONY: assimp
assimp: libassimp.a

.PHONY: dep_dir	
dep_dir: dependencies

dependencies:
	mkdir dependencies

# download of assimp
.PHONY: assimp_download
assimp_download: dep_dir dependencies/assimp # pre-requisites; true-target

ASSIMP_URL = https://github.com/assimp/assimp.git
# The reason for choosing such an obsolete version:
#     1. latest versions(v5.2.3+) have a weird bug when built in my environment,
#        the memory layout of aiMaterialProperty will be wrong, which is fatal
#        for retrieving material properties later.
#     2. slightly-earlier versions(v5.0.0) fail to build in my environment
#     3. versions before that(v4.0.0 and v4.1.0) generate humougous binaries(~200MB)
#        which is a not good for a small project like this.
# So I finally went back to v3.3.1, which have none of the above problems.
# Note: the location of files needed to be copied for installation may differ from version to version.
ASSIMP_VERSION = v3.3.1
dependencies/assimp: dependencies
	cd dependencies && git clone $(ASSIMP_URL)
	cd dependencies/assimp

# build of assimp
.PHONY: assimp_build
assimp_build: assimp_download dependencies/assimp/build dependencies/assimp/build/code/libassimp.a

ASSIMP_CMAKE_ARGS = -B build -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release -D BUILD_SHARED_LIBS=OFF
dependencies/assimp/build: assimp_download
	cd dependencies/assimp && \
	git checkout $(ASSIMP_VERSION) && \
	cmake $(ASSIMP_CMAKE_ARGS)

NUM_WORKERS = 12
dependencies/assimp/build/code/libassimp.a: dependencies/assimp/build
	cd dependencies/assimp/build && \
	make -j $(NUM_WORKERS)

# install of assimp - always redo the copy
# higher versions need this:
#   cp dependencies/assimp/build/lib/libassimp.a lib/libassimp.a
#   cp dependencies/assimp/include/assimp include/assimp -r
#   cp dependencies/assimp/build/include/assimp/config.h include/assimp/config.h
# TODO: make this indenpendent of the exact path
.PHONY: assimp_install
assimp_install: assimp_build
	rm -f lib/libassimp.a
	cp dependencies/assimp/build/code/libassimp.a lib/libassimp.a
	rm -f lib/libzlibstatic.a
	cp dependencies/assimp/build/contrib/zlib/libzlibstatic.a lib/libzlibstatic.a
	rm -rf include/assimp
	cp dependencies/assimp/include/assimp include/assimp -r
	
.PHONY: assimp_clean
assimp_clean:
	rm -rf dependencies/assimp/build
