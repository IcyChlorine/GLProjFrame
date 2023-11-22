CXX = g++
CXXFLAGS = -std=c++11
DEBUG = false

SRC_PATH = ./src
LIB_PATH = ./lib
INCLUDE_PATH = ./include

SRC_FILES = $(wildcard $(SRC_PATH)/*.cpp $(SRC_PATH)/*.c)
# 链接glfw3时必须同时链接gdi32
# 另外，注意依赖顺序。
LIBRARIES = -lglfw3 -lgdi32 -lfreetype -lassimp.dll

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

OBJ_FILES = $(SRC_FILES:.cpp=.o)

main.exe: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(TARGET_FILENAME) -I $(INCLUDE_PATH) -L $(LIB_PATH) $(LIBRARIES)

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

AUXS = makefile
count:
	cloc $(SRC_FILES) $(AUXS)