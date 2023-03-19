CXX = g++
CXXFLAGS = -std=c++11

.PHONY: test
test: test.exe

test.exe: src/test.cpp src/test.h
	$(CXX) $(CXXFLAGS) src/test.cpp -o test.exe

.PHONY: clean
clean:
	rm -f test.exe