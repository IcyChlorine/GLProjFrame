CXX = g++
CXXFLAGS = -std=c++11
DEBUG = true

SRC_PATH = ./src
LIB_PATH = ./lib
INCLUDE_PATH = ./include
BUILD_PATH = ./build

HEADERS = $(wildcard $(SRC_PATH)/*.h)
SRCS = $(wildcard $(SRC_PATH)/*.cpp)
SRCS_THIRD = $(wildcard $(SRC_PATH)/third/*.cpp $(SRC_PATH)/third/*.c)
# 链接glfw3时必须同时链接gdi32
# 另外，注意依赖顺序。
LIBS = -lglfw3 -lgdi32 -lfreetype -lassimp -liconv -lzlibstatic
OBJS = $(patsubst $(SRC_PATH)/%.cpp, $(BUILD_PATH)/%.o, $(wildcard $(SRC_PATH)/*.cpp))
OBJS_THIRD = $(patsubst $(SRC_PATH)/third/%.cpp, $(BUILD_PATH)/%.o, $(wildcard $(SRC_PATH)/third/*.cpp))
OBJS_THIRD+= $(patsubst $(SRC_PATH)/third/%.c,   $(BUILD_PATH)/%.o, $(wildcard $(SRC_PATH)/third/*.c))
#OBJS+= $(patsubst $(SRC_PATH)/%.c,  $(BUILD_PATH)/%.o, $(wildcard $(SRC_PATH)/*.c))

TARGET_EXEC = main.exe

# some little matic
ifeq ($(DEBUG), true)
	CXXFLAGS += -Og -g
else
	CXXFLAGS += -O2
endif

# for incremental compilation #
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -I $(INCLUDE_PATH) -c $< -o $@

$(BUILD_PATH)/%.o: $(SRC_PATH)/%.c $(HEADERS)
	$(CXX) $(CXXFLAGS) -I $(INCLUDE_PATH) -c $< -o $@

$(BUILD_PATH)/%.o: $(SRC_PATH)/third/%.cpp
	$(CXX) $(CXXFLAGS) -I $(INCLUDE_PATH) -c $< -o $@

$(BUILD_PATH)/%.o: $(SRC_PATH)/third/%.c
	$(CXX) $(CXXFLAGS) -I $(INCLUDE_PATH) -c $< -o $@

$(TARGET_EXEC): $(OBJS) $(OBJS_THIRD)
	$(CXX) $(CXXFLAGS) $(OBJS) $(OBJS_THIRD) -o $(TARGET_EXEC) -I $(INCLUDE_PATH) -L $(LIB_PATH) $(LIBS)

# macroscopic tasks #
.PHONY: main all clean run count

main: main.exe
# pass

all: main
# pass

clean:
	rm -f $(TARGET_EXEC)
	rm -f build/*.o

run: all
	./$(TARGET_EXEC)

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
#           like "cp .../build/lib/libxxx.a lib/libxxx.a"

# Note: .PHONY targets should only work as FINAL wrappers.
#       If a (real) target depends on a .PHONY target, then the target will ALWAYS be executed,
#       even if the target itself is not PHONY.

.PHONY: dep assimp freetype
dep: dep_dir assimp freetype
	@echo "Building dependencies..."

.PHONY: assimp
assimp: libassimp.a

dependencies:
	mkdir dependencies

# download of assimp
.PHONY: assimp_download
assimp_download: dependencies/assimp # pre-requisites; true-target

ASSIMP_URL = https://github.com/assimp/assimp.git
ASSIMP_VER = v5.2.3
dependencies/assimp:
	mkdir -p dependencies
	cd dependencies && git clone $(ASSIMP_URL)
	cd dependencies/assimp

# build of assimp
.PHONY: assimp_build
assimp_build: dependencies/assimp/build/lib/libassimp.dll.a

ASSIMP_CMAKE_ARGS = -B build -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release -D BUILD_SHARED_LIBS=ON
dependencies/assimp/build: dependencies/assimp
	cd dependencies/assimp && \
	git checkout $(ASSIMP_VER) && \
	cmake $(ASSIMP_CMAKE_ARGS)

NUM_WORKERS = 12
dependencies/assimp/build/lib/libassimp.dll.a: dependencies/assimp/build
	cd dependencies/assimp/build && \
	make assimp -j $(NUM_WORKERS)

# install of assimp - always redo the copy
# for v3.3.1
#   rm -f lib/libzlibstatic.a
#	cp dependencies/assimp/build/contrib/zlib/libzlibstatic.a lib/libzlibstatic.a
#	rm -rf include/assimp
#	cp dependencies/assimp/include/assimp include/assimp -r
# TODO: make this indenpendent of the exact path
# TODO: how can I get rid of the "-5" in the name of libassimp-5.dll?
#       That makes the rules dependent on the version of assimp.
.PHONY: assimp_install
# depend on assimp_build
assimp_install: dependencies/assimp/build/lib/libassimp.dll.a
	rm -f lib/libassimp.dll.a lib/libassimp.a
	cp dependencies/assimp/build/lib/libassimp.dll.a lib/libassimp.dll.a
	rm -f libassimp-5.dll
	cp dependencies/assimp/build/bin/libassimp-5.dll libassimp-5.dll
	rm -f lib/libzlibstatic.a
	cp dependencies/assimp/build/contrib/zlib/libzlibstatic.a lib/libzlibstatic.a
	rm -rf include/assimp
	cp dependencies/assimp/include/assimp include/assimp -r
	cp dependencies/assimp/build/include/assimp/config.h include/assimp/config.h
	
	
.PHONY: assimp_clean
assimp_clean:
	rm -rf dependencies/assimp/build

FREETYPE_URL = https://gitlab.freedesktop.org/freetype/freetype.git
FREETYPE_VER = VER-2-13-0
dependencies/freetype:
	mkdir -p dependencies
	cd dependencies && git clone $(FREETYPE_URL)
	cd dependencies/freetype

.PHONY: freetype_download
freetype_download: dependencies/freetype

FREETYPE_CMAKE_ARGS = -B build -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release
.PHONY: freetype_build
freetype_build: dependencies/freetype/build/libfreetype.a

dependencies/freetype/build: dependencies/freetype
	cd dependencies/freetype && \
	git checkout $(FREETYPE_VER) && \
	cmake $(FREETYPE_CMAKE_ARGS)

dependencies/freetype/build/libfreetype.a: dependencies/freetype/build
	cd dependencies/freetype/build && \
	make -j $(NUM_WORKERS)

.PHONY: freetype_install
freetype_install: dependencies/freetype/build/libfreetype.a
	rm -f lib/libfreetype.a
	cp dependencies/freetype/build/libfreetype.a lib/libfreetype.a
	rm -f include/ft2build.h
	cp dependencies/freetype/include/ft2build.h include/ft2build.h
	rm -rf include/freetype
	cp dependencies/freetype/include/freetype include/freetype -r