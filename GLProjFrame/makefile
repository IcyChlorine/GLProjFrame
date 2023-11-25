CXX = g++
CXXFLAGS = -std=c++11
DEBUG = true

SRC_PATH = ./src
LIB_PATH = ./lib
INCLUDE_PATH = ./include

HEADERS = $(wildcard $(SRC_PATH)/*.h)
SRCS = $(wildcard $(SRC_PATH)/*.cpp $(SRC_PATH)/*.c)
# 链接glfw3时必须同时链接gdi32
# 另外，注意依赖顺序。
LIBS = -lglfw3 -lgdi32 -lfreetype -lassimp.dll -liconv

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

.PHONY: dep assimp
dep: assimp
	@echo "Building dependencies..."

.PHONY: assimp
assimp: libassimp.a
	@echo "Building assimp..."

	mkdir dependencies
	cd dependencies
	git clone https://github.com/assimp/assimp.git
	cd assimp
	cmake -B build -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release -D BUILD_SHARED_LIBS=OFF
	cd build
	make -j 12
	# ->assimp->dependencies->GLProjFrame/GLProjFrame
	cd ../../..
	rm lib/libassimp.a
	cp dependencies/assimp/build/lib/libassimp.a lib/libassimp.a
	rm -rf include/assimp
	cp dependencies/assimp/build/include/assimp include/assimp -r

dependencies:
	mkdir dependencies

ASSIMP_VERSION = v5.2.3 # some versions, such as v5.0.0, can't build
dependencies/assimp: dependencies
	cd dependencies && git clone https://github.com/assimp/assimp.git
	cd dependencies/assimp && git checkout $(ASSIMP_VERSION)

ASSIMP_CMAKE_ARGS = -B build -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release -D BUILD_SHARED_LIBS=OFF
dependencies/assimp/build: dependencies/assimp
	cd dependencies/assimp && \
	cmake $(ASSIMP_CMAKE_ARGS)

NUM_WORKERS = 12
dependencies/assimp/build/lib/libassimp.a: dependencies/assimp/build
	cd dependencies/assimp/build && \
	make -j $(NUM_WORKERS)

.PHONY: assimp_install
assimp_install:
	rm lib/libassimp.a
	cp dependencies/assimp/build/lib/libassimp.a lib/libassimp.a
	rm -rf include/assimp
	cp dependencies/assimp/build/include/assimp include/assimp -r

