CXX = g++
CXXFLAGS = -O2 -std=c++11 -Wall
DEBUG = false

SRC_PATH = ./src
LIB_PATH = ./lib
INCLUDE_PATH = ./include

SRC_FILES = $(wildcard $(SRC_PATH)/*.cpp $(SRC_PATH)/*.c)
# 链接glfw3时必须同时链接gdi32
# 另外，注意依赖顺序。
LIBRARIES = -lglad -lglfw3 -lgdi32

TARGET_FILENAME = main.exe

# some little matic
ifeq ($(DEBUG), true)
CXXFLAGS += -g
endif

main: $(SRC_FILES)
	$(CXX) $(CXXFLAGS) $(SRC_FILES) -o $(TARGET_FILENAME) -I $(INCLUDE_PATH) -L $(LIB_PATH) $(LIBRARIES)

all: main
	#pass

.PHONY: clean
clean:
	rm -f $(TARGET_FILENAME)

run: all
	./$(TARGET_FILENAME)